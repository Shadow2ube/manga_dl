//
// Created by christian on 8/8/22.
//

#ifndef MANGA_DL_SRC_MDLX_MDLX_COMMAND_H_
#define MANGA_DL_SRC_MDLX_MDLX_COMMAND_H_

#include <string>
#include <sstream>
#include "../util.h"
#include "../printers.h"

auto mdlx_download(const std::string &url, bool quiet = true) {
  std::string filename = "mdltmp/" + random_string(10);
  std::stringstream ss;
  ss << "wget "
     << (quiet ? "--quiet \"" : "\"")
     << url
     << "\" -O \""
     << filename
     << "\"";

  system(ss.str().c_str());
  return filename;
}

auto mdlx_find_all_reg(const std::string &in, const std::string &regex) {
  auto x = find_in_file(in, std::regex(regex));
//  print(x);
  return x;
}

auto mdlx_find_all_xp(const std::string &in, const std::string &xpath) -> std::vector<std::string> {
  return {"UNIMPLEMENTED"};
}

#endif //MANGA_DL_SRC_MDLX_MDLX_COMMAND_H_
