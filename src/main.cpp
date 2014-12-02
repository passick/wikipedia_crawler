#include <vector>
#include <iostream>

#include "Crawler.h"
#include "WikiPage.h"

int main()
{
  std::vector<char> banned_symbols = { ':' };
  Crawler wiki_crawler("./data2/",
      "http://simple.wikipedia.org",
      "/wiki",
      banned_symbols);
  wiki_crawler.Crawl("/wiki/Main_Page");
  
  return 0;
}
