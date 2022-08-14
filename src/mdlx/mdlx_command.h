//
// Created by christian on 8/8/22.
//

#ifndef MANGA_DL_SRC_MDLX_MDLX_COMMAND_H_
#define MANGA_DL_SRC_MDLX_MDLX_COMMAND_H_

#include <string>
#include <sstream>
#include <pcap/socket.h>
#include "../../lib/json.hpp"
#include "../util.h"
#include "../printers.h"
#include "../http.h"
#include "../webdriver/webdriver.h"

using json = nlohmann::json;

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

auto mdlx_find_all_reg(const std::string &in, const std::string &regex) -> std::vector<std::string> {
  auto x = find_in_file(in, std::regex(regex));
  print(x);
  return x;
}

auto mdlx_find_all_xp(const std::string &in, const std::string &xpath) -> std::vector<std::string> {
  return {"UNIMPLEMENTED"};
}

auto mdlx_click_reg(const std::string &in, const std::string &regex) -> std::string {
  return "UNIMPLEMENTED";
}

auto mdlx_click_xp(const std::string &in, const std::string &xpath) -> std::string {
  webdriver wd;
  wd.open(in);
  auto button = wd.findByXpath(xpath);
  wd.executeScript("arguments[0].click();", {button});
  auto res = wd.getPage();
  return res;
}

#endif //MANGA_DL_SRC_MDLX_MDLX_COMMAND_H_
