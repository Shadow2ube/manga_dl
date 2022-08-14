#include <iostream>
#include <string>
#include <curl/curl.h>
#include "constants.h"
#include "util.h"
#include "mdlx/mdlx_parse.h"

using json = nlohmann::json;

auto main(int argc, char **argv) -> int {
  if (argc < 2) {
    std::cout << "Invalid arguments\nmanga-dl <full-url>" << std::endl;
  }

//  json res = http::post("http://localhost:4444/session", {"content-type: application/json"}, constants::wd_post_body);
//  webdriver_sessionID = res["value"]["sessionId"];

  curl_global_init(CURL_GLOBAL_ALL);

  auto *tm = unmined::task_manager<constants::task_manager_workers>::get_instance();
  tm->set(unmined::KILL_ON_EMPTY, false);
  tm->task_start_callback = [](const unmined::task &task, int wid) {
    printf("[%i]: Task '%s' started\n", wid, task.name.c_str());
  };
  tm->task_stop_callback = [](const unmined::task &task, int wid) {
    printf("[%i]: Task '%s' finished\n", wid, task.name.c_str());
  };
  tm->task_fail_callback = [](const unmined::task &task, int wid, int err) {
    printf("[%i]: Task '%s' failed (%i)\n", wid, task.name.c_str(), err);
  };

  system("mkdir -p mdltmp");

  for (int i = 1; i < argc; ++i) {
    std::string full_url = argv[i];
    const std::string domain = regexp(
        regexp(full_url, constants::domain_exp, 1),
        std::regex(R"((?:[A-Za-z0-9]+\.)?([A-Za-z0-9]+)(\.[A-Za-z0-9]+))"), 1);
    parse_mdlx("sites/mangapark.mdlx", full_url);
  }
  tm->set(unmined::KILL_ON_EMPTY, true);
  curl_global_cleanup();
  return 0;
}
