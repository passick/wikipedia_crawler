#ifndef WIKIPAGE_H
#define WIKIPAGE_H

#include <string>

#include "HTMLContent.h"
#include "HTMLTag.h"

class WikiPage
{
  std::string title_;
  std::string text_;

 public:
  WikiPage(HTMLTag& tag);
  const std::string& title();
  const std::string& text();
};

#endif
