#include <pthread.h>

#include <string>

#include "Indexer.h"
#include "PageRanker.h"

const int number_of_threads = 5;

int main()
{
  PageRanker ranker("./data3/", "./data3/files_map");
  return 0;
  Indexer wiki_indexer("./data3/",
      "./index2/",
      "./index2/indexed_files");
  pthread_t threads[number_of_threads];
  for (int i = 0; i < number_of_threads; ++i)
  {
    int result = pthread_create(&threads[i], NULL,
        Indexer::MultithreadIndex, static_cast<void *>(&wiki_indexer));
    if (result)
    {
      std::cerr << "Could not create indexer thread number " << i << std::endl;
      std::cerr << "pthread_create's return value: " << result << std::endl;
      exit(1);
    }
  }
  for (int i = 0; i < number_of_threads; ++i)
  {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
