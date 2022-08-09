//
// Created by christian on 6/22/22.
//

#ifndef MANGA_DL_SRC_UTIL_H_
#define MANGA_DL_SRC_UTIL_H_

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <array>
#include <regex>
#include "constants.h"
#include "printers.h"

inline auto exec(const std::string &cmd) {
  std::array<char, 512> buffer{};
  std::string result;
  std::vector<std::string> out;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    std::string _t = buffer.data();
    _t.pop_back();
    out.emplace_back(_t);
  }
  return out;
}

template<typename T>
auto remove_dupes(std::vector<T> vec) {
  std::vector<T> out = vec;
  std::sort(out.begin(), out.end());
  auto last = std::unique(out.begin(), out.end());
  out.erase(last, out.end());
  return out;
}

inline auto regexp(const std::string &in, const std::regex &regex) {
  std::smatch sm;
  std::regex_search(in, sm, regex);
  return sm[0];
}

inline auto regexp(const std::string &in, const std::regex &regex, int x) {
  std::smatch sm;
  std::regex_search(in, sm, regex);
  return sm[x];
}

inline auto find_in_string(const std::string &in, std::regex regex) {
  std::smatch sm;
  std::vector<std::string> out;
  std::string content = in;
  while (std::regex_search(content, sm, regex)) {
    out.push_back(sm.str());
    content = sm.suffix();
  }
  return remove_dupes(out);
}

inline auto find_in_file(const std::string &filename, const std::regex &regex, bool remove_file=true) {
  std::smatch sm;
  std::ifstream file(filename);
  std::vector<std::string> out;
  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  while (std::regex_search(content, sm, constants::mangapark_chapter_exp)) {
    out.push_back(sm.str());
    content = sm.suffix();
  }

  file.close();
  if (remove_file) system(("rm " + filename).c_str());
  return remove_dupes(out);
}

inline auto download(const std::string &url, const std::string &filename, bool quiet = false) {
  std::stringstream ss;
  ss << "wget "
     << (quiet ? "--quiet \"" : "\"")
     << url
     << "\" -O \""
     << filename
     << "\"";
  system(ss.str().c_str());
}

inline auto is_num(const std::string &in) {
  return std::all_of(in.begin(), in.end(), [](char c) {
//    std::cout << c << ": " << std::isdigit(c) << std::endl;
    return std::isdigit(c);
  });
}

inline auto remove(std::string &in, std::initializer_list<char> delims) {
  for (auto i: delims) {
    size_t pos;
    while ((pos = in.find(i)) != std::string::npos) {
      in.replace(pos, 1, "");
    }
  }
}

inline auto random_string(int size) {
  char *letters = "abcdefghijklmnopqrstuvwxyz";
  std::string out;
  srand(time(nullptr));
  for (int i = 0; i < size; ++i) {
    out += letters[rand() % 26];
  }
  return out;
}

inline auto read_file(const std::string &filename) {
  std::ifstream stream(filename);
  std::string out;
  std::string line;
  while (stream.good()) {
    std::getline(stream, line);
    out += line;
  }
  return out;
}

auto split(const std::string &in, const std::string &delim) {
  std::vector<std::string> out;
  size_t pos;
  std::string s = in;
  while ((pos = s.find(delim)) != std::string::npos) {
    std::string t = s.substr(0, pos);
    if (!t.empty()) out.push_back(t);
    s.erase(0, pos + delim.length());
  }
  if (!s.empty()) out.push_back(s);
  return out;
}

auto vects(const std::vector<std::string> &in) {
  std::string out;
  for (auto &i: in) out += i;
  return out;
}

#endif //MANGA_DL_SRC_UTIL_H_
