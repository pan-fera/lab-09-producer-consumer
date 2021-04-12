//
// Created by hacker on 4/11/21.
//

#ifndef TEMPLATE_DOWNLOADER_HPP
#define TEMPLATE_DOWNLOADER_HPP
#include <iostream>
#include <regex>
#include "Queue.hpp"
#include "ThreadPool.h"
#include "Parser.hpp"
struct Page{
  std::string page;
  std::string protocol;
  std::string host;
  size_t depth;
};
class Downloader {
 public:
  explicit Downloader() = delete;
  static void DownloadPage();
  inline static Queue<Page> queue_pages;
  //static std::atomic<bool> _finish;
 private:
  static void DownloadHttp(std::string&& host, std::string&& target, size_t depth);
  static void DownloadHttps(std::string&& host, std::string&& target, size_t depth);


};

#endif  // TEMPLATE_DOWNLOADER_HPP
