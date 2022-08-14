//
// Created by christian on 8/9/22.
//

#ifndef MANGA_DL_SRC_HTTP_H_
#define MANGA_DL_SRC_HTTP_H_

#include <string>
#include <list>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "printers.h"
#include "webdriver/webdriver.h"

using json = nlohmann::json;

namespace http {

inline auto request(const std::function<void(curlpp::Easy &)> &func) -> json {
  std::stringstream ss;
  try {
    curlpp::Cleanup cleanup;
    curlpp::Easy req;
    func(req);

//    req.setOpt<curlpp::options::Verbose>(true);

    ss << req;
  } catch (curlpp::LogicError &e) {
    printe("logic err");
    printe(e.what());
  } catch (curlpp::RuntimeError &e) {
    printe("runtime err");
    printe(e.what());
  }
  json out = json::parse(ss.str());
  return out;
}

inline auto post(const std::string &url, std::vector<std::string> headers, const json &data) -> json {
  return request([&](curlpp::Easy &req) {
    req.setOpt<curlpp::options::Url>(url);
    req.setOpt<curlpp::options::HttpHeader>(std::list<std::string>(headers.begin(), headers.end()));
    req.setOpt<curlpp::options::PostFields>(data.dump());
  });
}

inline auto get(const std::string &url) -> json {
  return request([=](curlpp::Easy &req) {
    req.setOpt<curlpp::options::Url>(url);
  });
}

inline auto del(const std::string &url) -> json {
  return request([=](curlpp::Easy &req) {
    req.setOpt<curlpp::options::Url>(url);
    req.setOpt<curlpp::options::CustomRequest>("DELETE");
  });
}

}

#endif //MANGA_DL_SRC_HTTP_H_
