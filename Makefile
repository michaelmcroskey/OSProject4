CXX=		g++
CXXFLAGS=	-g -Wall -std=gnu++11
SHELL=		bash
PROGRAMS=	tool libcurl

all:		tool libcurl

tool:	tool.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

libcurl:		libcurl.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) *.csv
	rm -r *.dSYM