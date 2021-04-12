//
// Created by hacker on 4/11/21.
//
#include <regex>
#include "Queue.hpp"
#include "ThreadPool.h"
#include "Parser.hpp"
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <cstdlib>
#include <string>
#include "root_certificates.hpp"
#include "Downloader.hpp"

using tcp = boost::asio::ip::tcp;     // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;     // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;  // from <boost/beast/http.hpp>
namespace beast = boost::beast;

void parse_url(Page& _page, std::string& url) {
  size_t p_s = url.find_first_of(':');
  _page.protocol = url.substr(0, p_s);
  size_t h_s = url.find('/', p_s + 3);

  if (h_s == std::string::npos) {
    _page.host = url.substr(h_s + 3, url.size() - p_s - 2);
    _page.target = "/";
  } else {
    _page.host = url.substr(p_s + 3, h_s - p_s - 3);
    _page.target = url.substr(h_s, url.size() - h_s + 1);
  }
}

void Downloader::DownloadPage() {
  if (!Parser::queue_url.empty()) {
    URL _tmp = Parser::queue_url.front();
    std::string url = _tmp.url;

    std::regex rx(R"(^http[s]?://.*)");
    if (!regex_match(url.begin(), url.end(), rx)) {
      Parser::queue_url.pop();
      return;
    }
    Page _page{};
    parse_url(_page, url);
    _page.depth = _tmp.depth;
    if (_page.protocol == "http") DownloadHttp(std::move(_page));
    if (_page.protocol == "https") DownloadHttps(std::move(_page));
    Parser::queue_url.pop();
  }
}

void Downloader::DownloadHttp(Page&& _page) {
  try {
    boost::asio::io_context ioc{};

    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(_page.host, "80");

    stream.connect(results);

    http::request<http::string_body> req{http::verb::get, _page.target, 11};
    req.set(http::field::host, _page.host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;

    http::read(stream, buffer, res);

    beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

    _page.page = res.body();
    queue_pages.push(std::move(_page));

    if (ec == boost::asio::error::eof) {
      ec.assign(0, ec.category());
    }
  } catch (...) {
  }
}

void Downloader::DownloadHttps(Page&& _page) {
  try {
    auto const port = "443";  // https - 443, http - 80
    auto const version = 11;

    boost::asio::io_context ioc;
    ssl::context ctx{ssl::context::sslv23_client};

    load_root_certificates(ctx);
    tcp::resolver resolver{ioc};
    ssl::stream<tcp::socket> stream{ioc, ctx};

    auto const results = resolver.resolve(_page.host, port);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req{http::verb::get, _page.target,
                                         version};
    req.set(http::field::host, _page.host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;

    http::read(stream, buffer, res);

    _page.page = res.body();

    queue_pages.push(std::move(_page));
    boost::system::error_code ec;
    stream.shutdown(ec);
  } catch (...) {
  }
}
