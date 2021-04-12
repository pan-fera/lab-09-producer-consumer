//
// Created by hacker on 4/11/21.
//

#ifndef TEMPLATE_PARSER_HPP
#define TEMPLATE_PARSER_HPP
#include <iostream>
#include "Queue.hpp"

struct URL{
  std::string url;
  int depth;
};
class Parser{
 public:
  explicit Parser() = delete;
  static int parse();
  inline static Queue<URL> queue_url;
  inline static Queue<std::string> queue_writer;

};

#endif  // TEMPLATE_PARSER_HPP
