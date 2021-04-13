//
// Copyright [2021] <pan_fera>
//

#ifndef INCLUDE_DOWNLOADER_HPP_
#define INCLUDE_DOWNLOADER_HPP_
#include <iostream>
#include "Downloader.hpp"
#include "Queue.hpp"
#include <string>

struct Page{
  std::string page;
  std::string protocol;
  std::string host;
  std::string target;
  size_t depth;
};

class Downloader {
 public:
  Downloader() = delete;
  static void DownloadPage();
  inline static Queue<Page> queue_pages;

 private:
  static void DownloadHttp(Page&& _page);
  static void DownloadHttps(Page&& _page);
};

#endif  // INCLUDE_DOWNLOADER_HPP_
