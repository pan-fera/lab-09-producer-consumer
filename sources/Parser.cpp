//
// Copyright [2021] <pan_fera>
//

#include "Parser.hpp"
#include <string>
#include <algorithm>
#include <regex>
#include <gumbo.h>

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

  if (gumbo_get_attribute(&node->v.element.attributes, "href"))
    href = gumbo_get_attribute(&node->v.element.attributes, "href");
  else if (gumbo_get_attribute(&node->v.element.attributes, "content"))
    href = gumbo_get_attribute(&node->v.element.attributes, "content");
  else if (node->v.element.tag == GUMBO_TAG_IMAGE ||
           node->v.element.tag == GUMBO_TAG_IMG)
    href = gumbo_get_attribute(&node->v.element.attributes, "src");

  if (href) {
    std::regex rx(R"((^http[s]?://.*)|(/.*))");
    std::string tmp = href->value;
    if (!regex_match(tmp.begin(), tmp.end(), rx)) return;

    if (href->value[0] == '/') {
      tmp = p.protocol + p.host + href->value;
    } else {
      tmp = href->value;
    }

    if (isImage(tmp)) {
      Parser::queue_writer.push(std::move(tmp));
    } else {
      if (p.depth == 1) return;
      URL _url{tmp, p.depth - 1};
      Parser::queue_url.push(std::move(_url));
    }
  }

  GumboVector* children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    search_for_links(static_cast<GumboNode*>(children->data[i]), p);
  }
}

void Parser::parse() {
  try {
    if (!Downloader::queue_pages.empty()) {
      Page _tmp = Downloader::queue_pages.front();
      GumboOutput* output = gumbo_parse(_tmp.page.c_str());  // const char*
      search_for_links(output->root, _tmp);
      gumbo_destroy_output(&kGumboDefaultOptions, output);
      Downloader::queue_pages.pop();
    }
  } catch (...) {
  }
}
