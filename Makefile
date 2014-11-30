CXX=clang++
CPPFLAGS=--std=c++11

SRCS=src/main.cpp src/Crawler.cpp src/Downloader.cpp \
	 src/SaveableStringContainer.cpp src/HTMLContent.cpp src/HTMLTag.cpp \
	 src/WikiPage.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
EXECUTABLE=crawler

LDFLAGS=

all: $(SRCS) $(EXECUTABLE)

debug: CXX += -DDEBUG -g
debug: all

$(EXECUTABLE): $(OBJS)
	  $(CXX) $(CPPFLAGS) $(OBJS) -o $(EXECUTABLE) $(LDFLAGS)

src/Crawler.o: src/Crawler.h src/Crawler.cpp src/Downloader.h \
  src/HTMLContent.h src/HTMLTag.h src/WikiPage.h
src/Downloader.o: src/Downloader.h src/Downloader.cpp
src/SaveableStringContainer.o: src/SaveableStringContainer.h \
  src/SaveableStringContainer.cpp
src/HTMLContent.o: src/HTMLContent.h src/HTMLContent.cpp src/HTMLTag.h
src/HTMLTag.o: src/HTMLTag.h src/HTMLTag.cpp src/HTMLContent.h
src/WikiPage.o: src/WikiPage.h src/WikiPage.cpp src/HTMLContent.h src/HTMLTag.h

clean:
	  $(RM) $(OBJS) $(EXECUTABLE)
