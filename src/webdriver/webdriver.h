//
// Created by christian on 8/11/22.
//

#ifndef MANGA_DL_SRC_WEBDRIVER_H_
#define MANGA_DL_SRC_WEBDRIVER_H_

#include <string>
#include <list>
#include "../../lib/json.hpp"

using json = nlohmann::json;
using namespace nlohmann::json_literals;

class webdriver {
 private:
  int port_;
  std::string sessionID_;
  std::string base_url_ = "http://127.0.0.1";
  pid_t pid_;

 public:
  webdriver();
  ~webdriver();

  auto post(
      const std::string &endpoint,
      const json &body) -> json;
  auto get(const std::string &endpoint) -> json;
  auto del(const std::string &endpoint) -> json;

  auto open(const std::string &url) -> json;

  auto findByXpath(const std::string &xpath) -> json;
  auto click(const std::string &elementId) -> json;
  auto executeScript(const std::string &script, std::vector<json> args) -> json;
  auto getPage() -> std::string;
};

#endif //MANGA_DL_SRC_WEBDRIVER_H_
