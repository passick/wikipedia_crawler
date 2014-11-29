CXX=clang++
CPPFLAGS=--std=c++11

SRCS=src/main.cpp src/Crawler.cpp src/Downloader.cpp src/SaveableStringContainer.cpp src/HTMLContent.cpp src/HTMLTag.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
EXECUTABLE=crawler

LDFLAGS=

all: $(SRCS) $(EXECUTABLE)

debug: CXX += -DDEBUG -g
debug: all

$(EXECUTABLE): $(OBJS)
	  $(CXX) $(CPPFLAGS) $(OBJS) -o $(EXECUTABLE) $(LDFLAGS)

src/Crawler.o: src/Crawler.h src/Crawler.cpp
src/Downloader.o: src/Downloader.h src/Downloader.cpp
src/main.o: src/Crawler.h
src/SaveableStringContainer.o: src/SaveableStringContainer.h src/SaveableStringContainer.cpp
src/HTMLContent.o: src/HTMLContent.h src/HTMLContent.cpp
src/HTMLTag.o: src/HTMLTag.h src/HTMLTag.cpp

clean:
	  $(RM) $(OBJS) $(EXECUTABLE)
