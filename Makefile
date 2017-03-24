CXX=		g++
CXXFLAGS=	-g -Wall -std=gnu++11 -lpthread -lcurl -static-libstdc++
SHELL=		bash
PROGRAMS=	site-tester

all:		site-tester

tool:	site-tester.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) csv/*.csv html/*.html
	rm -r *.dSYM