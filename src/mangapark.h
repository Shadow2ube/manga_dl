//
// Created by christian on 6/22/22.
//

#ifndef MANGA_DL_SRC_MANGAPARK_H_
#define MANGA_DL_SRC_MANGAPARK_H_

#include <string>
#include <regex>
#include <thread>
#include <future>
#include <iterator>
#include <condition_variable>
#include <numeric>
#include "util.h"
#include "constants.h"

#define MAX_CONCURRENT 15
#define MAX_PER_PDF 500
//#define MAX_PER_PDF 10

#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define CYN "\e[0;36m"
#define REDB "\e[41m"

namespace std { // ew tf is this
std::string to_string(std::string v) {
  return v;
}
}

template<typename T>
std::string join(std::vector<T> const &vec, std::string delim) {
  if (vec.empty()) return {};
  return std::accumulate(
      vec.begin() + 1,
      vec.end(),
      std::to_string(vec[0]),
      [&delim](const std::string &a, T b) { return a + delim + std::to_string(b); });
}

bool file_exists(const std::string &name) {
  return fopen(name.c_str(), "r");
}

bool chap_url_compare(const std::string &a, const std::string &b) {
  std::string ax = regexp(a, std::regex(R"(c\d+-\w{2})"));
  std::string bx = regexp(b, std::regex(R"(c\d+-\w{2})"));
  return std::stoll(ax.substr(1, ax.size() - 4)) < std::stoll(bx.substr(1, bx.size() - 4));
}

bool image_name_compare(const std::string &a, const std::string &b) {
  std::string ax = regexp(a, std::regex(R"(/\d+.jpeg)"));
  std::string bx = regexp(b, std::regex(R"(/\d+.jpeg)"));
  return std::stoll(ax.substr(1, ax.size() - 6)) < std::stoll(bx.substr(1, bx.size() - 6));
}

inline std::vector<std::string> get_images(const std::string &url, const std::string &domain, int i) {
  std::string chap_name = regexp(url, std::regex(R"(c\d+-\w\w)"));
  printf("getting images from chapter" RED " %s [%i]" NC "\n", chap_name.c_str(), i);
  std::cout.flush();
  return exec(std::string("python src/python/mod_mangapark.py ")
//  return exec(std::string("./src/python/dist/mod_mangapark ")
                  .append(domain)
                  .append(url));
}

inline void download_from_mangapark(const std::string &url, int idx = 0) {
  const std::string domain = regexp(url, constants::domain_exp);

  std::cout << "downloading from mangapark: " << url << std::endl;

  system("mkdir -p manga-dl_tmp");
  download(url, "manga-dl_tmp/page.html");

  std::cout << "finding urls in manga homepage" << std::endl;

  std::vector<std::string> chapters = find_in_file("manga-dl_tmp/page.html", constants::mangapark_chapter_exp);
  std::sort(chapters.begin(), chapters.end(), &chap_url_compare);

  std::cout << "found " << chapters.size() << " chapters" << std::endl;

  std::vector<std::vector<std::string>> results;
  for (int i = 0; i < chapters.size(); i += MAX_CONCURRENT) {
    int amt = chapters.size() - i < MAX_CONCURRENT ? chapters.size() - i : MAX_CONCURRENT;
    std::vector<std::future<std::vector<std::string>>> p;
    for (int j = 0; j < amt; ++j) {
      p.push_back(std::async(&get_images, chapters[i + j], domain, i + j));
    }
    for (auto &j: p) {
      results.push_back(j.get());
    }
  }

  std::vector<std::string> images_vec;
  size_t offset = 0;
  for (int i = 0; i < results.size(); ++i) {
    std::vector<std::string> v = results[i];
    std::vector<std::string> p;
    printf("downloading images from id: %d\n", i);
    std::vector<std::thread> downloads;
    for (int j = 0; j < v.size(); ++j) {
      std::string name = std::to_string(j + offset) + ".jpeg";
      printf("downloading %s from %s\n", name.c_str(), v[j].c_str());
      std::cout.flush();
      if (!file_exists("manga-dl_tmp/" + name)) // TODO: rework this u dingus, it'll catch anything dum dum
        downloads.emplace_back(std::thread(&download, v[j], "manga-dl_tmp/" + name, true)); // uncontrolled threading ew
      p.push_back("manga-dl_tmp/" + name + " ");
    }
    for (auto &j: downloads) j.join();

    images_vec.insert(images_vec.end(), p.begin(), p.end());
    offset += v.size();
  }

  std::sort(images_vec.begin(), images_vec.end(), &image_name_compare);

  std::vector<std::string> split;
  for (int i = 0; i < images_vec.size(); i += MAX_PER_PDF) {
    int amt = images_vec.size() - i < MAX_PER_PDF ? images_vec.size() - i : MAX_PER_PDF;
    std::string p;
    for (int j = 0; j < amt; ++j) p += images_vec[i + j] + " ";
    split.push_back(p);
  }

  std::vector<std::string> out;
  std::cout << "converting images to multiple small PDFs" << std::endl;
  for (int i = 0; i < split.size(); ++i) {
    std::cout << "creating manga-dl_tmp/tmp-"
              << std::to_string(idx)
              << "-"
              << std::to_string(i)
              << ".pdf"
              << std::endl;
    std::string cmd = "convert "
        + split[i]
        + " manga-dl_tmp/tmp-"
        + std::to_string(idx)
        + "-"
        + std::to_string(i)
        + ".pdf";
    out.push_back("manga-dl_tmp/tmp-"
                      + std::to_string(idx)
                      + "-"
                      + std::to_string(i)
                      + ".pdf ");

    system(cmd.c_str());
  }

  std::string merged = join(out, " ");

  std::cout << "merging PDFs: " << merged << std::endl;

  std::string cmd = "pdfunite " + merged + " manga-" + std::to_string(idx) + ".pdf";
  system(cmd.c_str());

  std::cout << "cleaning up" << std::endl;
  system("rm -r manga-dl_tmp");
}

#endif //MANGA_DL_SRC_MANGAPARK_H_
