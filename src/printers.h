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
auto print(std::vector<T> p) {
  for (auto &i: p) {
    std::cout << '"' << i << "\" ";
  }
  std::cout << std::endl;
}
template<typename T>
auto print(T in) {
  std::cout << in << std::endl;
}
auto print(std::vector<chain_t> chain) {
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



#endif //MANGA_DL_SRC_PRINTERS_H_
