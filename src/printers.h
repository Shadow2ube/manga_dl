//
// Created by christian on 8/8/22.
//

#ifndef MANGA_DL_SRC_PRINTERS_H_
#define MANGA_DL_SRC_PRINTERS_H_

#include <iostream>
#include <vector>
#include <variant>
#include "mdlx/mdlx_types.h"

using chain_t = std::variant<std::string, chain_type>;

template<typename T>
inline auto print(std::vector<T> p) {
  std::stringstream ss;
  for (auto &i: p) {
    ss << '"' << i << "\" ";
  }
  ss << '\n';
  printf("%s", ss.str().c_str());
  std::cout.flush();
}
template<typename T>
inline auto print(T in) {
  std::stringstream ss;
  ss << in;
  printf("%s\n", ss.str().c_str());
  std::cout.flush();
}
inline auto print(std::vector<chain_t> chain) {
  for (auto &i: chain) {
    if (std::holds_alternative<chain_type>(i)) {
      switch (std::get<chain_type>(i)) {
        case ct_PASS:std::cout << " ct_PASS ";
          break;
        case ct_FOR_EACH:std::cout << " ct_FOR_EACH ";
          break;
        case ct_MERGE:std::cout << " ct_MERGE ";
          break;
        case ct_ORDERED_FOR_EACH:std::cout << " ct_ORDERED_FOR_EACH ";
        default:break;
      }
    } else {
      std::cout << " " << std::get<std::string>(i) << " ";
    }
  }
  std::cout << std::endl;
}

template<typename T>
inline auto printe(T in) {
  std::stringstream ss;
  ss << in;
  fprintf(stderr, "*** %s ***\n", ss.str().c_str());
  std::cout.flush();
}

#endif //MANGA_DL_SRC_PRINTERS_H_
