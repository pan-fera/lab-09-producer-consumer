//
// Created by hacker on 4/11/21.
//

#ifndef TEMPLATE_PARSER_HPP
#define TEMPLATE_PARSER_HPP
#include <iostream>
#include "Downloader.hpp"
#include "Queue.hpp"

struct URL {
  std::string url;
  size_t depth;
};
class Parser {
 public:
  explicit Parser() = delete;
  static void parse();
  inline static Queue<URL> queue_url;
  inline static Queue<std::string> queue_writer;
};

#endif  // TEMPLATE_PARSER_HPP
