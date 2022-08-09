//
// Created by christian on 8/7/22.
// This solution is very garbo, sorry for anyone trying to optimize this
// It's probably better to rewrite this
//

#ifndef MANGA_DL_SRC_MDLXP_H_
#define MANGA_DL_SRC_MDLXP_H_

#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <variant>
#include <unordered_map>
#include "mdlx_command.h"
#include "../../lib/task_manager.hpp"
#include "mdlx_types.h"
#include "../printers.h"

using chain_t = std::variant<std::string, chain_type>;
using argument = std::variant<command, command_op, modifier, std::string, int>;
using arguments = std::vector<argument>;

auto str_to_chain_type(const std::string &in) {
  if (in == ":>") return ct_PASS;
  if (in == "=>") return ct_FOR_EACH;
  if (in == "==>") return ct_ORDERED_FOR_EACH;
  if (in == "->") return ct_MERGE;
  return ct_INVALID;
}
auto str_to_command(const std::string &in) {
  if (in == "download") return c_DOWNLOAD;
  if (in == "pass") return c_PASS;
  if (in == "find_all") return c_FIND_ALL;
  if (in == "click") return c_CLICK;
  if (in == "vec") return c_VEC;
  if (in == "merge") return c_MERGE;
  if (in == "attribute") return c_ATTRIBUTE;
  if (in == "contains") return c_CONTAINS;
  return c_INVALID;
}
auto str_to_command_op(const std::string &in) {
  if (in == "REGEX") return co_REGEX;
  if (in == "XPATH") return co_XPATH;
  if (in == "ORDERED") return co_ORDERED;
  return co_INVALID;
}
auto str_to_modifier(const std::string &in) {
  if (in == "--") return m_PASS;
  if (in == "@@") return m_ARG_ALL;
  if (in == "@0") return m_ARG_0;
  if (in == "@1") return m_ARG_1;
  if (in == "@2") return m_ARG_2;
  return m_INVALID;
}

auto parse_chain(const std::string &f_line, std::ifstream &stream) {
  std::deque<chain_t> out;
  std::string line = f_line;

  bool do_next_line = false;
  do {
    do_next_line = false;
    for (auto &word: split(line, " ")) {
      if (word == "\\") {
        do_next_line = true;
        continue;
      }
      chain_type c = str_to_chain_type(word);
      if (c != ct_INVALID) out.push_back(c);
      else out.push_back(word);
    }

    std::getline(stream, line);
  } while (!stream.eof() && do_next_line);

  return out;
}

auto parse_task(const std::string &l, const std::string &arg1) -> std::tuple<std::string, arguments> {
  std::string name;
  std::string line = l;
  std::vector<argument> args;
  std::string str;

  if (line.contains("\"")) {
    auto start = line.find_first_of('"');
    auto end = line.find_last_of('"');
    str = line.substr(start + 1, end - start - 1);
    line.replace(start, str.size() + 2, "");
  }
  std::vector<std::string> tokens = split(line, " ");
  name = tokens[0].substr(0, tokens[0].size() - 1);
  tokens.erase(tokens.begin(), tokens.begin() + 1);

  for (auto word: tokens) {
    auto c = str_to_command(word);
    if (c != c_INVALID) {
      args.push_back(c);
      continue;
    }
    auto co = str_to_command_op(word);
    if (co != co_INVALID) {
      args.push_back(co);
      continue;
    }
    auto m = str_to_modifier(word);
    if (m != m_INVALID) {
      if (m == m_ARG_1) args.push_back(arg1);
      else args.push_back(m);
      continue;
    }
    if (is_num(word)) {
      args.push_back(std::stoi(word));
      continue;
    }
    remove(word, {'"'});
    args.push_back(word);
  }

  args.push_back(str);
  return {name, args};
}

auto run_task(const std::string &name,
              const std::string &thru,
              const arguments &args) -> std::variant<std::string, std::vector<std::string>> {
  switch (std::get<command>(args[0])) {
    case c_PASS: return std::get<std::string>(args[1]);
    case c_DOWNLOAD: return mdlx_download(thru);
    case c_FIND_ALL: {
      if (std::get<command_op>(args[1]) == command_op::co_REGEX)
        return mdlx_find_all_reg(thru, std::get<std::string>(args[2]));
      else if (std::get<command_op>(args[1]) == command_op::co_XPATH)
        return mdlx_find_all_xp(thru, std::get<std::string>(args[2]));
    }
    case c_CLICK:break;
    case c_VEC:print("VEC");
//      return "";
      break;
    case c_MERGE:print("MERGE");
//      return "";
      break;
    case c_ATTRIBUTE:print("ATTRIBUTE");
//      return "";
      break;
    case c_CONTAINS:print("CONTAINS");
//      return "";
      break;
    case c_INVALID:print("INVALID");
//      return "";
      break;
  }
  return "";
}

void follow_chain_recurse(std::deque<chain_t> chain,
                          std::unordered_map<std::string, arguments> tasks,
                          std::variant<std::string, std::vector<std::string>> current,
                          int level = 1) {

  while (!chain.empty()) {
    chain_t ct = chain.front();
    chain.pop_front();

    if (std::holds_alternative<std::string>(ct)) {
      std::string name = std::get<std::string>(ct);
      auto args = tasks[name];
      current = run_task(name, std::get<std::string>(current), args);
      if (std::holds_alternative<std::vector<std::string>>(current)) {
        if (std::get<std::vector<std::string>>(current).empty()) exit(69);
      } else {
        if (std::get<std::string>(current).empty()) exit(69);
      }

    } else {
      switch (std::get<chain_type>(ct)) {
        case ct_PASS: break; // just skip, equivalent to passing the value along
        case ct_FOR_EACH:break;
        case ct_ORDERED_FOR_EACH: {
          auto *tm = unmined::task_manager<4>::get_instance();

          auto list = std::get<std::vector<std::string>>(current);
          for (int i = 0; i < list.size(); ++i) {
            tm->add({"tmp-" + std::to_string(level) + "-" + std::to_string(i),
                     [&chain, &tasks, &list, &i, &level]() -> int {
                       follow_chain_recurse(chain, tasks, list[i], level + 1);
                       print("yeet");
                       return 0;
                     }});
          }
          tm->start();
          tm->join(); // TODO: remove this

//          exit(69);
          break;
        }
        case ct_MERGE:exit(-69);
          break;
        case ct_INVALID: break;
      }
    }
  }
}

void follow_chain(std::deque<chain_t> chain,
                  std::unordered_map<std::string, arguments> tasks) {
  std::variant<std::string, std::vector<std::string>> current;

  while (!chain.empty()) {
    chain_t ct = chain.front();
    chain.pop_front();

    if (std::holds_alternative<std::string>(ct)) {
      std::string name = std::get<std::string>(ct);
      if (name == "out") {
        if (std::holds_alternative<std::vector<std::string>>(current))
          print(std::get<std::vector<std::string>>(current));
        else print(std::get<std::string>(current));
        return;
      }
      auto args = tasks[name];
      current = run_task(name, std::get<std::string>(current), args);

    } else {
      switch (std::get<chain_type>(ct)) {
        case ct_PASS: break; // just skip, equivalent to passing the value along
        case ct_FOR_EACH:break;
        case ct_ORDERED_FOR_EACH: {
          auto *tm = unmined::task_manager<4>::get_instance();

          auto list = std::get<std::vector<std::string>>(current);
          print(list.size());
          print(list);
          for (int i = 0; i < list.size(); ++i) {
            tm->add({"tmp-0-" + std::to_string(i),
                     [chain, tasks, list, i]() -> int {
                       follow_chain_recurse(chain, tasks, list[i]);
                       return 0;
                     }});
          }
          tm->start();
          tm->join(); // TODO: remove this

//          exit(69);
          break;
        }
        case ct_MERGE:exit(69420);
          break;
        case ct_INVALID: break;
      }
    }
  }
}

auto parse_mdlx(const std::string &uri, const std::string &arg1) -> void {
  std::deque<chain_t> chain;
  std::unordered_map<std::string, arguments> tasks;
  std::ifstream in(uri);
  std::string line;

  while (in.good()) {
    std::getline(in, line);
    if (line.empty() || line.starts_with("##")) continue;
    if (line.starts_with("CHAIN:")) {
      chain = parse_chain(line.substr(6, line.size() - 1), in);
    } else {
      auto [x, y] = parse_task(line, arg1);
      tasks[x] = y;
    }
  }
  in.close();

  follow_chain(chain, tasks);
}

#endif //MANGA_DL_SRC_MDLXP_H_
