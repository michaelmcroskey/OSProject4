// NOTE: BEFORE SUBMITTING MAKE SURE TO CHANGE COMPILER TO /usr/bin/g++

// OS Project 4
// 
// Michael McRoskey
// Maggie Thomann

#include <iostream>		// for cout
#include <fstream>		// for ofstream
#include <string>		// for string
#include <sstream>		// for sstream / split
#include <thread>

#include "SearchFile.h"
#include "SitesFile.h"
#include "ConfigFile.h"

using namespace std;

void *fetch_url( void *args );

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

// Thread Args --------------------------------

typedef struct __myarg_t {
	string url;
	string html;
} myarg_t;

// Main Execution  ----------------------------
int main(int argc, char *argv[]) {
	
	string line;
	
	// Check if enough arguments
	if (argc != 2)
		usage(EXIT_FAILURE);
	
	// Parse config, search, and sites files
	ConfigFile config (argv[1]);
	SearchFile search (config.getValue("SEARCH_FILE"));
	SitesFile sites (config.getValue("SITE_FILE"));
	
	// Display the configuration
	config.display();
	
	// Fetching Threads ----------------------
	int fetch = stoi(config.getValue("NUM_FETCH"));
		
	while (sites.more_urls){
		pthread_t* fetch_thread = new pthread_t[fetch];
		myarg_t* fetch_args = new myarg_t[fetch];
		
		// Initialize arguments for each args struct
		for (int i=0; i<fetch; i++){
			fetch_args[i].url = sites.top();
		}
		
		// Create threads
		for (int i=0; i<fetch; i++){
			if (pthread_create(&(fetch_thread[i]), NULL, fetch_url, (void *) (fetch_args+i))) {
				cout << "Error creating thread " << i << endl;
				exit(EXIT_FAILURE);
			}
		}
		
		// Join Threads
		for (int i=0; i<fetch; i++){
			if (pthread_join(fetch_thread[i], NULL)){
				cout << "Error joining thread " << i << endl;
				exit(EXIT_FAILURE);
			}
		}
	}
	
	// Parsing Threads -----------------------
	//int parse = stoi(config.getValue("NUM_PARSE"));
	//
	//
	//
	//
	//
	//
	//
	
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

void *fetch_url( void *args ){
	
	myarg_t *m  = (myarg_t*) args;
	
	cout << "Parsing: " << m->url << endl;
//	LibCurl url (m->url);
//	m->html = url.getString();
//	
	pthread_exit(NULL);
}
