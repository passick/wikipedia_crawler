#include <pthread.h>

#include <vector>
#include <iostream>

#include "Crawler.h"
#include "WikiPage.h"

const int THREADS_NUMBER = 20;

int main()
{
  Crawler wiki_crawler("./data3/",
      "http://simple.wikipedia.org",
      "/wiki/Main_Page");
  pthread_t threads[THREADS_NUMBER];
  for (int i = 0; i < THREADS_NUMBER; ++i)
  {
    int result = pthread_create(&threads[i], NULL,
        Crawler::MultithreadCrawl, static_cast<void *>(&wiki_crawler));
    if (result)
    {
      std::cerr << "Could not create thread number " << i << std::endl;
      std::cerr << "pthread_create's return value: " << result << std::endl;
      exit(1);
    }
  }
  for (int i = 0; i < THREADS_NUMBER; ++i)
  {
    pthread_join(threads[i], NULL);
  }
  return 0;
}
