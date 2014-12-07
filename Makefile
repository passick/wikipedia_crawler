CXX=clang++
CPPFLAGS=--std=c++11

COMMON_SRCS=
CRAWLER_SRCS=src/main.cpp src/Crawler.cpp src/Downloader.cpp \
			 src/SaveableStringContainer.cpp src/HTMLContent.cpp src/HTMLTag.cpp \
			 src/WikiPage.cpp
INDEXER_SRCS=src/IndexerUtility.cpp src/Indexer.cpp src/IndexerCache.cpp \
			 src/FilenameAndLink.cpp

COMMON_OBJS=$(subst .cpp,.o,$(COMMON_SRCS))
CRAWLER_OBJS=$(subst .cpp,.o,$(CRAWLER_SRCS))
INDEXER_OBJS=$(subst .cpp,.o,$(INDEXER_SRCS))

CRAWLER_EXECUTABLE=crawler
INDEXER_EXECUTABLE=indexer

COMMON_LDFLAGS=-lpthread
CRAWLER_LDFLAGS=
INDEXER_LDFLAGS=

all: $(COMMON_SRCS) $(CRAWLER_SRCS) $(INDEXER_SRCS) \
  $(CRAWLER_EXECUTABLE) $(INDEXER_EXECUTABLE)

debug: CXX += -DDEBUG -g
debug: all

$(CRAWLER_EXECUTABLE): $(COMMON_OBJS) $(CRAWLER_OBJS)
	  $(CXX) $(CPPFLAGS) $^ -o $@ $(COMMON_LDFLAGS) $(CRAWLER_LDFLAGS)

$(INDEXER_EXECUTABLE): $(COMMON_OBJS) $(INDEXER_OBJS)
	  $(CXX) $(CPPFLAGS) $^ -o $@ $(COMMON_LDFLAGS) $(INDEXER_LDFLAGS)

src/main.o: src/Crawler.h src/WikiPage.h src/Indexer.h
src/Crawler.o: src/Crawler.h src/Crawler.cpp src/Downloader.h \
  src/HTMLContent.h src/HTMLTag.h src/WikiPage.h
src/Downloader.o: src/Downloader.h src/Downloader.cpp
src/SaveableStringContainer.o: src/SaveableStringContainer.h \
  src/SaveableStringContainer.cpp
src/HTMLContent.o: src/HTMLContent.h src/HTMLContent.cpp src/HTMLTag.h
src/HTMLTag.o: src/HTMLTag.h src/HTMLTag.cpp src/HTMLContent.h
src/WikiPage.o: src/WikiPage.h src/WikiPage.cpp src/HTMLContent.h src/HTMLTag.h

src/Indexer.o: src/Indexer.h src/Indexer.cpp src/SaveableStringContainer.h \
  src/FilenameAndLink.h src/IndexerCache.h
src/IndexerUtility.o: src/IndexerUtility.cpp src/Indexer.h
src/IndexerCache.o: src/IndexerCache.h src/IndexerCache.cpp \
  src/FilenameAndLink.h
src/FilenameAndLink.o: src/FilenameAndLink.h src/FilenameAndLink.cpp

clean:
	  $(RM) $(COMMON_OBJS) $(CRAWLER_OBJS) $(INDEXER_OBJS) \
		$(CRAWLER_EXECUTABLE) $(INDEXER_EXECUTABLE)
