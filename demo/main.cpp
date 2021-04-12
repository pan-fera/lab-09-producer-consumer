#include "ThreadPool.h"
#include "Downloader.hpp"
#include "Parser.hpp"
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
po::options_description desc("Options");
desc.add_options()
("url", po::value<std::string>(), "адрес HTML страницы")
("depth", po::value<size_t>(), "глубина поиска по странице")
("network_threads", po::value<size_t>(), "количество потоков для скачивания страниц")
("parser_threads", po::value<size_t>(), "количество потоков для обработки страниц")
("output", po::value<std::string>(), "путь до выходного файл")
("help", "вспомогательное сообщение");

po::variables_map vm;
po::store(po::parse_command_line(argc, argv, desc), vm);
po::notify(vm);

if (vm.count("help")) {
std::cout << desc << std::endl;
return 0;
}

std::string url{};
size_t depth{};
size_t network_threads{};
size_t parser_threads{};
std::string output{};

url = (vm.count("url"))? vm["url"].as<std::string>() : "https://github.com/";
depth = (vm.count("depth"))? vm["depth"].as<size_t>() : 1;
network_threads = (vm.count("network_threads"))?  vm["network_threads"].as<size_t>() : 1;
parser_threads = (vm.count("parser_threads"))? vm["parser_threads"].as<size_t>() : 1;
output = (vm.count("output"))? vm["output"].as<std::string>() : "output.txt";

std::cout << url << " "<< depth << " "<< network_threads<< " "<< parser_threads<< " "<< output<< std::endl;
return 0;
}/*
int main()
{
  ThreadPool pool1(6);
  ThreadPool pool2(2);
  //a.DownloadPage("http://www.ctyme.com/intr/");
  URL url{"https://www.cyberforum.ru/boost-cpp/thread2383592.html", 2};
  Parser::queue_url.push(std::move(url));

  while (!Parser::queue_url.empty() || !Downloader::queue_pages.empty() ||
      Downloader::queue_pages._counter || Parser::queue_url._counter){
    pool1.enqueue([] { Downloader::DownloadPage(); });
    pool2.enqueue([] { Parser::parse(); });


  }
  while(!Parser::queue_writer.empty())
    std::cout << Parser::queue_writer.front() << std::endl;
  }
*/