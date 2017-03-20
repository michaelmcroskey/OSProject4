// NOTE: BEFORE SUBMITTING MAKE SURE TO CHANGE COMPILER TO /usr/bin/g++

// OS Project 4
// 
// Michael McRoskey
// Maggie Thomann

#include <iostream>		// for cout
#include <fstream>		// for ofstream
#include <string>		// for string
#include <sstream>		// for sstream / split

#include "SearchFile.h"
#include "SitesFile.h"
#include "ConfigFile.h"

using namespace std;

// Utility Functions --------------------------
void usage(int status) {
	cout << "usage: tool <filename>" << endl;
	exit(status);
}

void file_error(string filename){
	cout << "Error: Unable to open file "
		<< filename << "." << endl;
	exit(EXIT_FAILURE);
}

// Main Execution  ----------------------------
int main(int argc, char *argv[]) {
	
	string line;
	
	// Check if enough arguments
	if (argc != 2)
		usage(EXIT_FAILURE);
	
	ConfigFile config (argv[1]);
	
	// Display the configuration
//	config.display();
	
	SearchFile search (config.getValue("SEARCH_FILE"));
//	search.display();
	
	SitesFile sites (config.getValue("SITE_FILE"));
//	sites.display();
	
	// Output file
	string output_filename = "1.csv";
	ofstream output (output_filename);
	if (output.is_open()){
		output << "Time,Phrase,Site,Count\n";
		output << "03-12-17-01:10:05,Irish,http://www.nd.edu/,4\n";
		output << "03-12-17-01:10:05,Notre,http://www.nd.edu/,10\n";
		output.close();
	} else {
		file_error(output_filename);
	}	
	
	exit(EXIT_SUCCESS);
}

