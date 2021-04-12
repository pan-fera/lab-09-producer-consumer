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
    results.emplace_back(
        pool.enqueue([] {
          Parser::parse();
          return 5;
        }));
    results.emplace_back(
        pool.enqueue([] {
          Downloader::DownloadPage();
          return 5;
        }));
    //std::cout<<std::this_thread::get_id()<<" "<<Parser::queue_url.empty() << Downloader::queue_pages.empty()<<std::endl;
  }
  }
/*
#include "Downloader.hpp"
#include "gumbo.h"
#include "Parser.hpp"

int main(){
  Downloader a;
  //a.DownloadPage("http://www.ctyme.com/intr/");
  a.DownloadPage("https://github.com/marketplace");

  Parser b;
  b.parse();
  return 0;
}*/