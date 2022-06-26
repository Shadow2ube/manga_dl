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
#include "constants.h"

inline std::vector<std::string> exec(const std::string &cmd) {
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
std::vector<T> remove_dupes(std::vector<T> vec) {
  std::vector<T> out = vec;
  std::sort(out.begin(), out.end());
  auto last = std::unique(out.begin(), out.end());
  out.erase(last, out.end());
  return out;
}

inline std::string regexp(const std::string &in, const std::regex &regex) {
  std::smatch sm;
  std::regex_search(in, sm, regex);
  return sm[0];
}

inline std::vector<std::string> find_in_file(const std::string &filename, const std::regex &regex) {
  std::smatch sm;
  std::ifstream file(filename);
  std::vector<std::string> out;
  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  while (std::regex_search(content, sm, constants::mangapark_chapter_exp)) {
    out.push_back(sm.str());
    content = sm.suffix();
  }

  file.close();
  return remove_dupes(out);
}

inline void download(const std::string &url, const std::string &filename, bool quiet = false) {
  std::stringstream ss;
  ss << "wget "
     << (quiet ? "--quiet \"" : "\"")
     << url
     << "\" -O \""
     << filename
     << "\"";
  system(ss.str().c_str());
}

#endif //MANGA_DL_SRC_UTIL_H_
