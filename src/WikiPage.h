#ifndef WIKIPAGE_H
#define WIKIPAGE_H

#include <string>

#include "HTMLContent.h"
#include "HTMLTag.h"

class WikiPage
{
  std::string title_;
  std::string text_;
  std::vector<std::string> links_;

 public:
  WikiPage(const HTMLTag& tag);

  const std::string& title();
  const std::string& text();
  const std::vector<std::string>& links();

  static bool is_link_ok(const std::string& link);
  static std::string transform_link(const std::string& link);
};

#endif
