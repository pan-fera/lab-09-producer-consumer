//
// Created by hacker on 4/11/21.
//

#ifndef TEMPLATE_DOWNLOADER_HPP
#define TEMPLATE_DOWNLOADER_HPP
#include <iostream>
#include "Downloader.hpp"
#include "Queue.hpp"

struct Page{
  std::string page;
  std::string protocol;
  std::string host;
  std::string target;
  size_t depth;
};

class Downloader {
 public:
  explicit Downloader() = delete;
  static void DownloadPage();
  inline static Queue<Page> queue_pages;
 private:
  static void DownloadHttp(Page&& _page);
  static void DownloadHttps(Page&& _page);
};

#endif  // TEMPLATE_DOWNLOADER_HPP
