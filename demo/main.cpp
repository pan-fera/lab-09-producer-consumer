#include <iostream>
#include <vector>
#include <chrono>

#include "ThreadPool.h"

#include "Downloader.hpp"
#include "Parser.hpp"
int main()
{
  ThreadPool pool(4);
  //a.DownloadPage("http://www.ctyme.com/intr/");
  std::vector< std::future<int> > results;
  URL url{"https://github.com/marketplace", 2};
  Parser::queue_url.push(url);
  while (!(Parser::queue_url.empty() && Downloader::queue_pages.empty())) {
        pool.enqueue([] {
          Parser::parse();
        });
    pool.enqueue([] {
          Downloader::DownloadPage();
        });
  }
  }
