#include "ThreadPool.h"
#include "Downloader.hpp"
#include "Parser.hpp"
#include <fstream>

int main()
{
  ThreadPool pool1(1);
  ThreadPool pool2(1);
  //a.DownloadPage("http://www.ctyme.com/intr/");
  URL url{"https://github.com/", 2};
  Parser::queue_url.push(std::move(url));

  while (!Parser::queue_url.empty() || !Downloader::queue_pages.empty() ||
      Downloader::queue_pages._counter || Parser::queue_url._counter){
    try {
      pool1.enqueue([] { Parser::parse(); });
    } catch (...) {

    }
    try {
      pool2.enqueue([] { Downloader::DownloadPage(); });
    } catch (...) {

    }
    //std::cout<<Downloader::queue_pages._counter<<" "<<Parser::queue_url._counter<<std::endl;
  }
  while(!Parser::queue_writer.empty())
    std::cout << Parser::queue_writer.front() << std::endl;
  }
