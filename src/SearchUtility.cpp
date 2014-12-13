#include <iostream>
#include <string>
#include <vector>

#include "SearchEngine.h"
#include "FilenameAndLink.h"

void FindAndPrint(const std::string& query, SearchEngine& engine)
{
  std::vector<FilenameAndLink> result = engine.Find(query);
  for (const FilenameAndLink& found_page : result)
  {
    std::cout << found_page.link << std::endl;
  }
  std::cout << "End of results." << std::endl;
}

int main(int argc, char **argv)
{
  SearchEngine engine("./index2/", "./data3/sorted_articles");
  std::string query;
  bool no_interactive = false;
  for (int i = 1; i < argc; ++i)
  {
    std::string argument(argv[i]);
    if (argument == "--no-interactive" || argument == "-n")
    {
      no_interactive = true;
    }
    else
    {
      std::cout << argument << ":" << std::endl;
      FindAndPrint(std::string(argv[i]), engine);
    }
  }
  if (no_interactive)
  {
    return 0;
  }
  while (std::getline(std::cin, query))
  {
    FindAndPrint(query, engine);
  }

  return 0;
}
