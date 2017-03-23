// NOTE: BEFORE SUBMITTING MAKE SURE TO CHANGE COMPILER TO /usr/bin/g++
// -static-libstdc++

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
#include <mutex>			// for mutex
#include <chrono>
#include <condition_variable>

#include "SearchFile.h"
#include "SitesFile.h"
#include "ConfigFile.h"
#include "LibCurl.h"
#include "Parse.h"

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

//void fetch_and_parse(SearchFile *search, SitesFile *sites, int period, int fetch, int parse);

void handle_exit(int sig) {
	exit(EXIT_SUCCESS);
}

// Thread Args --------------------------------

typedef struct __myarg_t {
	string url;
	string html;
} myarg_t;

// Mutex and stuff ----------------------------

bool keepLooping = true;

queue<string> fetch_queue;
mutex fetch_queue_mutex;
condition_variable fetch_cv;

queue<string> parse_queue;
mutex parse_queue_mutex;
condition_variable parse_cv;

//void go(SitesFile *sites) {
//	unique_lock<mutex> lck(urls_mutex);
//	more_urls = sites->are_more_urls();
//	cv.notify_all();
//}

void _fetch(int id) {
	while(keepLooping){
		unique_lock<mutex> flck(fetch_queue_mutex);
			while (fetch_queue.size()==0)
				fetch_cv.wait(flck);
			string site_url = fetch_queue.front();
			cout << "popping " << site_url << endl; 
			fetch_queue.pop();
			fetch_cv.notify_all();
		flck.unlock();
		
		LibCurl url (site_url);
		
		unique_lock<mutex> plck(parse_queue_mutex);
			parse_queue.push(url.getString());
			parse_cv.notify_all();
		plck.unlock();
	}
}

//void _parse(int id) {
//	while (!more_urls)
//		this_thread::yield();
//		
//	for (volatile int i=0; i<1000000; ++i) {}
//	cout << id << endl;
//}

// Main Execution  ----------------------------
int main(int argc, char *argv[]) {
	
	// Catch CTRL-C
	signal(SIGINT, handle_exit);
	signal(SIGHUP, handle_exit);
		
	// Check if enough arguments
	if (argc != 2)
		usage(EXIT_FAILURE);
	
	// Parse config, search, and sites files
	ConfigFile config (argv[1]);
	//SitesFile sites (config.getValue("SITE_FILE"));
	//SearchFile search (config.getValue("SEARCH_FILE"));
	
	// Initialize period, # fetch and parse threads
	int period = stoi(config.getValue("PERIOD_FETCH"));
	int fetch = stoi(config.getValue("NUM_FETCH"));
	int parse = stoi(config.getValue("NUM_PARSE"));
	
//	parse_queue.push("http://www.google.com/");
//	parse_queue.push("http://www.yahoo.com/");
//	parse_queue.push("http://www.amazon.com/");
//	parse_queue.push("http://www.bing.com/");
//	parse_queue.push("http://www.ebay.com/");
	
	// Display the configuration
	config.display();
	
	while(1){
		
		cout << "|   10 seconds   |" << endl;
		
		string line, sites_filename = config.getValue("SITE_FILE");
		ifstream sites (sites_filename);
		if (sites.is_open()){
			while (getline(sites,line)){
				fetch_queue.push(line);
			}
			sites.close();
		} else {
			cout << "Error: Unable to open file "
				<< sites_filename << "." << endl;
			exit(EXIT_FAILURE);
		}
		
		// Create fetch threads  ----------------------------
		thread* fetch_threads = new thread[fetch];
		for (int i=0; i<fetch; ++i)
			fetch_threads[i] = thread(_fetch, i);
					
		for (int i=0; i<fetch; ++i) fetch_threads[i].join();
		
		delete[] fetch_threads;
		
		// Create parse threads  ----------------------------
//		thread* parse_threads = new thread[parse];
//		for (int i=0; i<parse; ++i)
//			parse_threads[i] = thread(_parse,i);
//			
//		for (int i=0; i<parse; ++i) parse_threads[i].join();
//		
//		delete[] parse_threads;
		
		this_thread::sleep_for(chrono::seconds(period));
	}
	
	
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


// while (gKeepRunning)
//	lock fqueue mutex
//		while (fQueue.getCount()==0)
//			pthread_cond_wait(mutex, cond_var)
// 		pop the first item from the queue
// 	unlock fqueue mutex
// 	CURL
// 	lock parse queue
// 		put data/work item in parse queue
// 		signal or bcast for cond_var
// 	unlock parse queue

//void *fetch_url( void *args ){
//	
//	myarg_t *m  = (myarg_t*) args;
//	
//	while(1){
//		while(!sites->are_more_urls())
//			pthread_cond_wait(this_thread, );
//		
//		m->url = sites.top();
//		LibCurl url (m->url);
//		m->html = url.getString();
//		
//		Parse parse;
//		parse.push(m->html);
//		pthread_cond_broadcast();
//		
//		parse.display();
//		
//		this_thread::sleep_for(period);
//	}
//	
//	pthread_exit(NULL);
//}

//void fetch_and_parse(SearchFile *search, SitesFile *sites, int period, int fetch, int parse){
//	cout << "|-------------------|" << endl;
//	cout << "|  NEW 10 SECONDS   |" << endl;
//	cout << "|-------------------|" << endl;
//
//	// reset queue here
//	
////	cout << endl;
////	cout << parse_queue.front().substr(0,100) << endl;
////	parse_queue.pop();
////	cout << endl;
////	cout << parse_queue.front().substr(0,100) << endl;
//	
//	// Parsing Threads -----------------------
//	//int parse = stoi(config.getValue("NUM_PARSE"));
//}