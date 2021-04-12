#include <vector>
#include "ThreadPool.h"

#include "Downloader.hpp"
#include "Parser.hpp"
#include <fstream>
int main()
{
  ThreadPool pool1(4);
  ThreadPool pool2(4);
  //a.DownloadPage("http://www.ctyme.com/intr/");
  URL url{"https://github.com/", 1};
  Parser::queue_url.push(url);

  while (!Parser::queue_url.empty() || !Downloader::queue_pages.empty() ||
      Downloader::queue_pages._counter || (Parser::queue_url._counter>=1)){
        pool1.enqueue([] {
          Parser::parse();
        });
    pool2.enqueue([] {
          Downloader::DownloadPage();
        });
    //std::cout<<Downloader::queue_pages._counter<<" "<<Parser::queue_url._counter<<std::endl;
  }
  while(!Parser::queue_writer.empty())
    std::cout << Parser::queue_writer.front() << std::endl;
  }
