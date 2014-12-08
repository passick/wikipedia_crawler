#include <string>
#include <iostream>

#include "PageRanker.h"

int main()
{
  std::cout << "Starting PageRanker." << std::endl;
  PageRanker ranker("./data3/", "./data3/files_map");
  ranker.FindProbabilities();
  std::cout << "Saving sorted list of articles." << std::endl;
  ranker.SaveProbabilities("./data3/sorted_articles");

  return 0;
}
