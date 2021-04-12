#include "ThreadPool.h"
#include "Downloader.hpp"
#include "Parser.hpp"
#include <fstream>

int main()
{
  ThreadPool pool1(6);
  ThreadPool pool2(2);
  //a.DownloadPage("http://www.ctyme.com/intr/");
  URL url{"https://github.com/", 2};
  Parser::queue_url.push(std::move(url));

  while (!Parser::queue_url.empty() || !Downloader::queue_pages.empty() ||
      Downloader::queue_pages._counter || Parser::queue_url._counter){
    pool1.enqueue([] { Downloader::DownloadPage(); });
    pool2.enqueue([] { Parser::parse(); });


  }
  while(!Parser::queue_writer.empty())
    std::cout << Parser::queue_writer.front() << std::endl;
  }
