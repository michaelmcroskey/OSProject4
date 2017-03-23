CXX=		g++
CXXFLAGS=	-g -Wall -std=gnu++11 -lpthread -lcurl
SHELL=		bash
PROGRAMS=	site-tester

all:		site-tester Libcurl.h ConfigFile.h

tool:	site-tester.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) *.csv
	rm -r *.dSYM