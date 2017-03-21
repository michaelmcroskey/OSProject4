// NOTE: BEFORE SUBMITTING MAKE SURE TO CHANGE COMPILER TO /usr/bin/g++

// OS Project 4
// 
// Michael McRoskey
// Maggie Thomann

#include <iostream>		// for cout
#include <fstream>		// for ofstream
#include <string>		// for string
#include <sstream>		// for sstream / split
#include <thread>		// for thread
#include <csignal>		// for signal
#include <unistd.h>		// for alarm
#include <queue>			// for parse_queue

#include "SearchFile.h"
#include "SitesFile.h"
#include "ConfigFile.h"
#include "LibCurl.h"

using namespace std;

void *fetch_url( void *args );

// Utility Functions --------------------------
void usage(int status) {
	cout << "usage: site-tester <filename>" << endl;
	exit(status);
}

void file_error(string filename){
	cout << "Error: Unable to open file "
		<< filename << "." << endl;
	exit(EXIT_FAILURE);
}

queue<string> parse_queue;

volatile sig_atomic_t print_flag = false;

void handle_alarm(int sig) {
	print_flag = true;
	alarm(12);
	signal(SIGALRM, handle_alarm);
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
	
	// Initialize alarm
	int period = stoi(config.getValue("PERIOD_FETCH"));
	
	// Every 180 seconds...
	signal(SIGALRM, handle_alarm);
	alarm(period);
	
//	while(true){
		cout << "|-------------------|" << endl;
		cout << "|  NEW 180 SECONDS  |" << endl;
		cout << "|-------------------|" << endl;

		if (print_flag) {
			cout << "Hello" << endl;
			print_flag = false;
			alarm(period);
		}
		
		// Fetching Threads ----------------------
		int fetch = stoi(config.getValue("NUM_FETCH"));
		
		while (sites.more_urls){
			cout << "	IN WHILE LOOP" << endl;
			
			pthread_t* fetch_thread = new pthread_t[fetch];
			myarg_t* fetch_args = new myarg_t[fetch];
			
			// Create threads
			for (int i=0; i<fetch; i++){
				fetch_args[i].url = sites.top();
				cout << "		Creating fetch thread: " << fetch_args[i].url << endl;
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
				parse_queue.push(fetch_args[i].html);
			}
			
			delete[] fetch_thread;
			delete[] fetch_args;
			cout << "	FINISHED LOOP" << endl;
		}
		
		cout << parse_queue.front() << endl;
		parse_queue.pop();
		cout << parse_queue.front() << endl;
		// Parsing Threads -----------------------
		//int parse = stoi(config.getValue("NUM_PARSE"));
		//
		//
		//
		//
		//
		//
		//
//	}
	
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
	
	// while (gKeepRunning)
	//	lock mutex
	//	while (fQueue.getCount()==0)
	//	pthread_cond_wait(mutex, cond_var)
	// 	pop the first item from the queue
	// 	unlock fqueue mutex
	// 	CURL
	// 	lock parse queue
	// 	put data/work item in parse queue
	// 	signal or bcast for cond_var
	// 	unlock parse queue
	
	myarg_t *m  = (myarg_t*) args;
	
	LibCurl url (m->url);
	m->html = url.getString();
	
	pthread_exit(NULL);
}
