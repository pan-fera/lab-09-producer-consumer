#include <boost/program_options.hpp>
#include <fstream>
#include "Downloader.hpp"
#include "Parser.hpp"
#include "ThreadPool.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::options_description desc("Options");
  desc.add_options()("url", po::value<std::string>(), "адрес HTML страницы")(
      "depth", po::value<size_t>(), "глубина поиска по странице")(
      "network_threads", po::value<size_t>(),
      "количество потоков для скачивания страниц")(
      "parser_threads", po::value<size_t>(),
      "количество потоков для обработки страниц")(
      "output", po::value<std::string>(), "путь до выходного файл")(
      "help", "вспомогательное сообщение");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  std::string url{};
  size_t depth;
  size_t network_threads;
  size_t parser_threads;
  std::string output{};

  url = (vm.count("url")) ?
  vm["url"].as<std::string>() : "https://github.com/";

  depth = (vm.count("depth")) ? vm["depth"].as<size_t>() : 1;

  network_threads = (vm.count("network_threads")) ?
  vm["network_threads"].as<size_t>() : 1;

  parser_threads = (vm.count("parser_threads")) ?
  vm["parser_threads"].as<size_t>() : 1;

  output = (vm.count("output")) ?
  vm["output"].as<std::string>() : "output.txt";

  ThreadPool pool_network(network_threads);
  ThreadPool pool_parser(parser_threads);
  URL _url{url, depth};
  Parser::queue_url.push(std::move(_url));

  while (!Parser::queue_url.empty() || !Downloader::queue_pages.empty() ||
         Downloader::queue_pages._counter || Parser::queue_url._counter) {
    pool_network.enqueue([] { Downloader::DownloadPage(); });
    pool_parser.enqueue([] { Parser::parse(); });
  }

  std::ofstream ofs{output};
  while (!Parser::queue_writer.empty()) {
    std::string _tmp = Parser::queue_writer.front();
    ofs << _tmp << std::endl;
    std::cout << _tmp << std::endl;
  }
  ofs.close();
  return 0;
}
