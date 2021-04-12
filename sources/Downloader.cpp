//
// Created by hacker on 4/11/21.
//

#include "Downloader.hpp"
#include "Queue.hpp"
#include "root_certificates.hpp"
#include <cstdlib>
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <string>
#include <fstream>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include "Parser.hpp"

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace beast = boost::beast;



void parse_url(std::string& protocol, [[maybe_unused]] std::string& host, [[maybe_unused]] std::string& target, std::string& url){
size_t p_s = url.find_first_of(':');
protocol = url.substr(0, p_s);
size_t h_s = url.find('/', p_s + 3);

  if (h_s == std::string::npos) {
    host = url.substr(h_s + 3, url.size() - p_s - 2);
    target = "/";
  } else {
    host = url.substr(p_s + 3, h_s - p_s - 3);
    target = url.substr(h_s, url.size() - h_s + 1);
  }
std::cout<<protocol<<" "<<host<<" "<<target<<std::endl;
}

void Downloader::DownloadPage() {
  //std::cout<<"tyt"<<std::endl;
  if(!Parser::queue_url.empty()) {
    std::string url = Parser::queue_url.front().url;

    std::regex rx(R"(^http[s]?://.*)");
    if (!regex_match(url.begin(), url.end(), rx))
      throw std::runtime_error("Wrong url");

    std::string protocol;
    std::string host;
    std::string target;
    parse_url(protocol, host, target, url);
    if (protocol == "http") DownloadHttp(std::move(host), std::move(target),Parser::queue_url.front().depth);
    if (protocol == "https") DownloadHttps(std::move(host), std::move(target), Parser::queue_url.front().depth);
    Parser::queue_url.pop();
  }
}

void Downloader::DownloadHttp(std::string&& host, std::string&& target, int depth) {
  try {
    boost::asio::io_context ioc{};

    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(host, "80");

    stream.connect(results);

    http::request<http::string_body> req{
        http::verb::get, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;

   http::read(stream, buffer, res);

    beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);


    Page _page {res.body(), "http://", host,  depth};
  //TODO::change depth
     queue_pages.push(_page);
    std::ofstream ofs{"out.txt"}; // запись html-страницы в файл
    ofs << res;
    ofs.close();
    if (ec == boost::asio::error::eof) {
      ec.assign(0, ec.category());
    }
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    // return EXIT_FAILURE;
  }
}

void Downloader::DownloadHttps(std::string&& host,  std::string&& target, int depth) {
  try
  {
    auto const port = "443"; // https - 443, http - 80
    auto const version = 11;

    boost::asio::io_context ioc;
    ssl::context ctx{ ssl::context::sslv23_client };

    load_root_certificates(ctx);
    tcp::resolver resolver{ ioc };
    ssl::stream<tcp::socket> stream{ ioc, ctx };

    auto const results = resolver.resolve(host, port);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req{ http::verb::get, target, version };
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    //boost::beast::flat_buffer buffer;
    // http::response<http::dynamic_body> res;

    beast::flat_buffer buffer;
    http::response<http::string_body> res;

    http::read(stream, buffer, res);

    Page _page {res.body(), "https://", host,  depth};
    //TODO::change depth
    queue_pages.push(_page);
    std::ofstream ofs{"out.txt"}; // запись html-страницы в файл
    ofs << res;
    ofs.close();

    // Gracefully close the stream
    boost::system::error_code ec;
    stream.shutdown(ec);
    if (ec == boost::asio::error::eof)
    {
      ec.assign(0, ec.category());
    }

  }
  catch (std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    //return EXIT_FAILURE;
  }

}

