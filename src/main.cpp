#include "Crawler.h"

int main()
{
  Crawler wiki_crawler("./data/",
      "http://simple.wikipedia.org",
      "/wiki");
  wiki_crawler.Crawl("/wiki/Main_Page");

  return 0;
}
