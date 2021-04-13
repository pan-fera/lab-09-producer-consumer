//
// Copyright [2021] <pan_fera>
//

#ifndef INCLUDE_PARSER_HPP_
#define INCLUDE_PARSER_HPP_
#include <iostream>
#include "Downloader.hpp"
#include "Queue.hpp"
#include <string>
struct URL {
  std::string url;
  size_t depth;
};
class Parser {
 public:
  Parser() = delete;
  static void parse();
  inline static Queue<URL> queue_url;
  inline static Queue<std::string> queue_writer;
};

#endif  // INCLUDE_PARSER_HPP_
