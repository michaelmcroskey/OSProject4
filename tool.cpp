// OS Project 4
// 
// Michael McRoskey
// Maggie Thomann

#include <iostream>		// for cout
#include <fstream>		// for ofstream
#include <string>		// for string
#include <map>			// for map
#include <sstream>		// for sstream / split
#include <vector>		// for split
#include <iterator>		// for split

#include "SearchFile.h"
#include "SitesFile.h"

using namespace std;

// Utility Functions --------------------------
void usage(int status) {
	cout << "usage: tool <filename>" << endl;
	exit(status);
}

// String splitting
template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim))
		*(result++) = item;
}
// String splitting
vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

// Create map of configuration variables
void parse_config_vars(string line, map<string, string> &config_vars){
	vector<string> args = split(line, '=');
	string key = args[0];
	string value = args[1];
	if (config_vars.find(key) == config_vars.end()){
		cout << "Warning: unknown parameter " << key << "." << endl;
	} else {
		config_vars[key] = value;
	}	
	return;
}

// Initialize map to default values
void init_map(map<string, string> &config_vars){
	// Define constants
	string PERIOD_FETCH = "180";
	string NUM_FETCH = "1";
	string NUM_PARSE = "1";
	string SEARCH_FILE = "";
	string SITE_FILE = "";
	
	config_vars["PERIOD_FETCH"] = PERIOD_FETCH;
	config_vars["NUM_FETCH"] = NUM_FETCH;
	config_vars["NUM_PARSE"] = NUM_PARSE;
	config_vars["SEARCH_FILE"] = SEARCH_FILE;
	config_vars["SITE_FILE"] = SITE_FILE;
	
	return;
}

void file_error(string filename){
	cout << "Error: Unable to open file "
		<< filename << "." << endl;
	exit(EXIT_FAILURE);
}

// Main Execution  ----------------------------
int main(int argc, char *argv[]) {
	
	map<string,string> config_vars;
	init_map(config_vars);
	string line;
	
	// Check if enough arguments
	if (argc != 2){
		usage(EXIT_FAILURE);
	
	// Parse config file
	} else {
		string config_filename = argv[1];
		ifstream config (config_filename);
		if (config.is_open()){
			while (getline(config,line)){
				parse_config_vars(line, config_vars);
			}
			config.close();
		} else {
			file_error(config_filename);
			exit(EXIT_FAILURE);
		}
	}
	
	// Display the configuration
	cout << "configuration: " << endl;
	for (auto& attribute : config_vars) {
		cout << attribute.first << "=" << attribute.second << endl;
	}
	
	SearchFile search(config_vars["SEARCH_FILE"]);
	search.display();
	
	SitesFile site(config_vars["SITE_FILE"]);
	site.display();
	
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

