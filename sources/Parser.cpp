//
// Created by hacker on 4/11/21.
//

#include "Parser.hpp"

#include <fstream>
#include <string>

#include "Downloader.hpp"
#include "gumbo.h"
bool isImage(const std::string& url) {
  size_t lastDotPos = url.find_last_of('.');
  if (lastDotPos == std::string::npos) return false;
  std::string ext = url.substr(lastDotPos + 1, url.size() - 1);

  static const std::vector<std::string> imageExtensions = {
      "apng", "avif", "gif",  "jpg", "jpeg", "jfif", "pjpeg", "pjp",
      "png",  "svg",  "webp", "bmp", "ico",  "cur",  "tif",   "tiff"};

  return std::any_of(imageExtensions.begin(), imageExtensions.end(),
                     [&](const std::string& s) { return ext == s; });
}

static void search_for_links(GumboNode* node, Page p) {
  if (node->type != GUMBO_NODE_ELEMENT) {
    return;
  }
  GumboAttribute* href = nullptr;

  //  if (node->v.element.tag == GUMBO_TAG_A &&
  if (gumbo_get_attribute(&node->v.element.attributes, "href"))
    href = gumbo_get_attribute(&node->v.element.attributes, "href");
  else if (gumbo_get_attribute(&node->v.element.attributes, "content"))
    href = gumbo_get_attribute(&node->v.element.attributes, "content");
  else if (node->v.element.tag == GUMBO_TAG_IMAGE ||
           node->v.element.tag == GUMBO_TAG_IMG)
    href = gumbo_get_attribute(&node->v.element.attributes, "src");
  if (href) {
    // if (!(node->v.element.tag == GUMBO_TAG_IMAGE || node->v.element.tag ==
    // GUMBO_TAG_IMG || isImage(href->value)))
    //   return;
    // std::cout << href->value <<std::endl;

    std::regex rx(R"((^http[s]?://.*)|((/?[^\0]\|/).*))");
    std::string tmp = href->value;
    if (!regex_match(tmp.begin(), tmp.end(), rx)) return;
    if (href->value[0] == '/') {
      tmp = p.protocol + p.host + href->value;
       //std::cout<<tmp<<std::endl;
    } else {
      tmp = href->value;
       //std::cout << tmp << std::endl;
    }
    if (isImage(tmp)) {
      std::cout << tmp << std::endl;
      Parser::queue_writer.push(tmp);
    } else {
      if (p.depth -1 == 0) return;
      URL _url{tmp, p.depth - 1};
      Parser::queue_url.push(_url);
    }
  }

  GumboVector* children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    search_for_links(static_cast<GumboNode*>(children->data[i]), p);
  }
}

void Parser::parse() {
  //std::cout<<"tyt"<<std::endl;
  if (!Downloader::queue_pages.empty()) {
    GumboOutput* output =
        gumbo_parse(Downloader::queue_pages.front().page.c_str());
    search_for_links(output->root, Downloader::queue_pages.front());
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    Downloader::queue_pages.pop();
  }
}
