//
// Created by christian on 8/11/22.
//

#include <thread>
#include <sys/wait.h>

#include "webdriver.h"
#include "../util.h"
#include "../http.h"

using namespace std::chrono_literals;

webdriver::webdriver() {
  port_ = get_available_port();
  pid_ = system2(("geckodriver --port " + std::to_string(port_)).c_str());
  base_url_ += ":" + std::to_string(port_);
  std::this_thread::sleep_for(5ms); // TODO: do something about this wait
//  sessionID_ = post("/session")["value"]["sessionId"];
//  json body = {
//      {"capabilities", {
//          {"alwaysMatch", {
//              {"acceptInsecureCerts", true},
//              {"moz:firefoxOptions", {
//                  {"args", {
//                      {"-headless"},
//                      {"prefs", {
//                          {"dom.ipc.processCount", 8}
//                      }}}
//                  }
//              }},
//          }}
//      }}
//  };
  json body = R"(
{
  "capabilities": {
    "alwaysMatch": {
      "acceptInsecureCerts": true,
      "moz:firefoxOptions": {
        "args": ["-headless"]
      }
    }
  }
}
)"_json;
  auto res = post("/session", body);
  sessionID_ = res["value"]["sessionId"];
}
webdriver::~webdriver() {
  del("/session/" + sessionID_);
  std::this_thread::sleep_for(10ms);
  kill(pid_, SIGINT);
  waitpid(pid_, nullptr, 0);
}

auto webdriver::post(const std::string &endpoint, const json &body) -> json {
  return http::post(base_url_ + endpoint,
                    {
                        "Content-Type: application/json",
                        "Content-Type: 'application/json; charset=UTF-8",
                        "User-Agent: manga-dl (C++)"
                    },
                    body);
}
auto webdriver::get(const std::string &endpoint) -> json {
  return http::get(base_url_ + endpoint);
}
auto webdriver::del(const std::string &endpoint) -> json {
  return http::del(base_url_ + endpoint);
}

auto webdriver::open(const std::string &url) -> json {
  json j;
  j["url"] = url;
  return post("/session/" + sessionID_ + "/url", j);
}
auto webdriver::findByXpath(const std::string &xpath) -> json {
  json body = {
      {"using", "xpath"},
      {"value", xpath},
      {"sessionId", sessionID_},
  };
  return post("/session/" + sessionID_ + "/element", body)["value"];
}
auto webdriver::click(const std::string &elementId) -> json {
  json body = {
      {"id", elementId},
  };
  print(body);
  return post("/session/" + sessionID_ + "/element/" + elementId + "/click", body);
}
auto webdriver::executeScript(const std::string &script, std::vector<json> args) -> json {
  json body = {
      {"script", script},
      {"args", args}
  };
  return post("/session/" + sessionID_ + "/execute/sync", body);

}
auto webdriver::getPage() -> std::string {
  return get("/session/" + sessionID_ + "/source")["value"];
}
