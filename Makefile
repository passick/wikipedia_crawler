CXX=g++
CPPFLAGS=--std=c++11 -O2

SRCS=src/main.cpp src/Crawler.cpp src/Downloader.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
EXECUTABLE=crawler

LDFLAGS=

all: $(SRCS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	  $(CXX) $(CPPFLAGS) $(OBJS) -o $(EXECUTABLE) $(LDFLAGS)

src/Crawler.o: src/Crawler.h src/Crawler.cpp
src/Downloader.o: src/Downloader.h src/Downloader.cpp
src/main.o: src/Crawler.h

clean:
	  $(RM) $(OBJS) $(EXECUTABLE)
