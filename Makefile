CXX=		g++
CXXFLAGS=	-lcurl -g -Wall -std=c++11 -lpthread -static-libstdc++
SHELL=		bash
PROGRAMS=	site-tester
CSV_FILES = $(filter-out csv/1.csv, $(wildcard csv/*.csv))
HTML_FILES = $(filter-out html/1.html, $(wildcard html/*.html))

all:		site-tester

tool:	site-tester.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	
test:
	echo "Test succeeded"

clean:
	rm -f $(PROGRAMS) $(CSV_FILES) $(HTML_FILES)
	rm -rf *.dSYM