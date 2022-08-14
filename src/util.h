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
#include <csignal>
#include <fcntl.h>
#include <netinet/in.h>
#include "constants.h"
#include "printers.h"

inline auto exec(const std::string &cmd) {
  std::array<char, 512> buffer{};
//  std::string result;
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

//inline auto system2(const char *command, int *infp, int *outfp) -> pid_t {
inline auto system2(const char *command) -> pid_t {
  int p_stdin[2];
  int p_stdout[2];
  pid_t pid;

  if (pipe(p_stdin) == -1)
    return -1;

  if (pipe(p_stdout) == -1) {
    close(p_stdin[0]);
    close(p_stdin[1]);
    return -1;
  }

  pid = fork();

  if (pid < 0) {
    close(p_stdin[0]);
    close(p_stdin[1]);
    close(p_stdout[0]);
    close(p_stdout[1]);
    return pid;
  } else if (pid == 0) {
    close(p_stdin[1]);
    dup2(p_stdin[0], 0);
    close(p_stdout[0]);
    dup2(p_stdout[1], 1);
    dup2(::open("/dev/null", O_RDONLY), 2);
    /// Close all other descriptors for the safety sake.
    for (int i = 3; i < 4096; ++i)
      ::close(i);

    setsid();
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(1);
  }

  close(p_stdin[0]);
  close(p_stdout[1]);

//  if (infp == nullptr) {
//    close(p_stdin[1]);
//  } else {
//    *infp = p_stdin[1];
//  }

//  if (outfp == nullptr) {
//    close(p_stdout[0]);
//  } else {
//    *outfp = p_stdout[0];
//  }

  return pid;
}

inline auto execs(const std::string &cmd) {
  std::array<char, 512> buffer{};
  std::string res;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  fgets(buffer.data(), buffer.size(), pipe.get());
  res = buffer.data();
  return res;
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

inline auto find_in_string(const std::string &in, const std::regex &regex) {
  std::smatch sm;
  std::vector<std::string> out;
  std::string content = in;
  while (std::regex_search(content, sm, regex)) {
    out.push_back(sm.str());
    content = sm.suffix();
  }
  return remove_dupes(out);
}

inline auto find_in_file(const std::string &filename, const std::regex &regex, bool remove_file = true) {
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
  std::string letters = "abcdefghijklmnopqrstuvwxyz";
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

inline auto split(const std::string &in, const std::string &delim) {
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

inline auto vects(const std::vector<std::string> &in) {
  std::string out;
  for (auto &i: in) out += i;
  return out;
}

inline auto get_available_port() -> int {
  struct sockaddr_in addr{};
  socklen_t len = sizeof(addr);
  addr.sin_port = 0;
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printe("socket()");
    return -1;
  }
  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
    printe("bind()");
    return -1;
  }
  if (getsockname(sock, (struct sockaddr *) &addr, &len) != 0) {
    printe("getsockname()");
    return -1;
  }
  return addr.sin_port;
}

#endif //MANGA_DL_SRC_UTIL_H_
