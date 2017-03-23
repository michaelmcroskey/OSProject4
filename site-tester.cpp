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
#include <queue>			// for queue
#include <mutex>			// for mutex
#include <ctime>			// for time
#include <unistd.h>		// for sleep
#include <condition_variable> // condition_variable

#include "ConfigFile.h"	// for config
#include "LibCurl.h"		// for url

using namespace std;

// Global variables ---------------------------
queue<string> fetch_queue;
mutex fetch_queue_mutex;
condition_variable fetch_cv;

queue<pair<string,string>> parse_queue;
mutex parse_queue_mutex;
condition_variable parse_cv;

bool outputting = false;
bool keep_looping = true;
vector< vector<string> > results;

// Utility Functions --------------------------
void usage(int status) {
	cout << "usage: site-tester <config_filename>" << endl;
	cout << "config_filename parameters:" << endl;
		cout << "\tPERIOD_FETCH=<int::period>" << endl;
		cout << "\tNUM_FETCH=<int::number_of_fetch_threads>" << endl;
		cout << "\tNUM_PARSE=<int::number_of_parse_threads>" << endl;
		cout << "\tSEARCH_FILE=<string::search_terms_filename>" << endl;
		cout << "\tSITE_FILE=<string::sites_filename>" << endl;
	exit(status);
}

void file_error(string filename){
	cout << "Error: Unable to open file "
		<< filename << "." << endl;
	exit(EXIT_FAILURE);
}

// returns current time as string
string current_time(){
	time_t currentTime;
	struct tm *localTime;

	time( &currentTime );                   // Get the current time
	localTime = localtime( &currentTime );  // Convert the current time to the local time

	int Day    = localTime->tm_mday;
	int Month  = localTime->tm_mon + 1;
	int Year   = localTime->tm_year + 1900;
	int Hour   = localTime->tm_hour;
	int Min    = localTime->tm_min;
	int Sec    = localTime->tm_sec;

	string date = to_string(Month) + "-" + to_string(Day) + "-" + to_string(Year) + " " + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec);
	return date;
}

// returns if files specified in config file exist
bool no_files(ConfigFile *config){
	ifstream search (config->getValue("SEARCH_FILE"));
	if (!search.is_open()) return true;
	ifstream sites (config->getValue("SITE_FILE"));
	if (!sites.is_open()) return true;
	return false;
}

// gracefully handle CRTL-C or SIGHUP
void handle_exit(int sig) {
	while(outputting) ;
	keep_looping = false;
	fetch_cv.notify_all();
	parse_cv.notify_all();
	cout << endl;
	cout << "Exiting" << endl;
	exit(EXIT_SUCCESS);
}

// Mutex functions ----------------------------

void stop_fetching() {
	sleep(15);
	fetch_cv.notify_all();
}

void stop_parsing() {
	sleep(15);
	parse_cv.notify_all();
}

// fetch threads enter and fetch websites and
// output html into parse_queue
void _fetch(int id) {
	// Keep looping until all urls are processed
	while(keep_looping){
		unique_lock<mutex> flck(fetch_queue_mutex);
			while (fetch_queue.size()==0){
				fetch_cv.wait(flck);
				return;
			}
			string site_url = fetch_queue.front();
			cout << "Thread " << id << ":\tfetch_pop \t" << site_url << endl; 
			fetch_queue.pop();
			fetch_cv.notify_all();
		flck.unlock();
		
		LibCurl url (site_url);
		
		unique_lock<mutex> plck(parse_queue_mutex);
			cout << "Thread " << id << ":\tparse_push\t" << site_url << endl;
			parse_queue.push(pair<string,string>(site_url,url.getString()));
			parse_cv.notify_all();
		plck.unlock();
	}
}

// parse threads enter and pop from parse_queue and 
// perform a word count
void _parse(int id, vector<pair<string, int> > queries) {
	while(keep_looping){
		unique_lock<mutex> plck(parse_queue_mutex);
			while (parse_queue.size()==0){
				parse_cv.wait(plck);
				return;
			}
			string url = parse_queue.front().first;
			string page = parse_queue.front().second;
			parse_queue.pop();
			cout << "Thread " << id << ":\tparse_pop \t" << url << endl;
			parse_cv.notify_all();
		plck.unlock();
		
		for (auto& query : queries){
			int count = 0;
			size_t pos = page.find(query.first, 0); // fist occurrence
			while(pos != string::npos){
				count++;
				pos = page.find(query.first, pos+1);
			}
			query.second = count;
			vector<string> result_line = {current_time(), query.first, url, std::to_string(query.second)};
			results.push_back(result_line);
		}
	}
}

// outputs data from parse() function to csv files
void output_to_file(string output_filename){
	outputting = true;
	// Output file
	ofstream output (output_filename);
	if (output.is_open()){
		output << "Time,Phrase,Site,Count\n";
		for (auto line : results){
			bool first = true;
			for (auto element : line){
				if (first){
					output << element;
					first = false;
				} else {
					output << "," << element;
				}
			}
			output << "\n";
		}
		output.close();
	} else {
		file_error(output_filename);
	}
	cout << "[Outputting to file " << output_filename << "]" << endl;
	results.clear();
	outputting = false;
}

// Main Execution  ----------------------------
int main(int argc, char *argv[]) {
	
	// Catch CTRL-C
	signal(SIGINT, handle_exit);
	signal(SIGHUP, handle_exit);
		
	// Check if enough arguments
	if (argc != 2)
		usage(EXIT_FAILURE);
	
	// Parse config file
	ConfigFile config (argv[1]);
	
	// Initialize period, # fetch and parse threads
	int period = stoi(config.getValue("PERIOD_FETCH"));
	int fetch = stoi(config.getValue("NUM_FETCH"));
	int parse = stoi(config.getValue("NUM_PARSE"));
	
	// Error checking for invalid parameters
	if (fetch<0 || parse<0 || fetch>8 || parse>8 || no_files(&config)){
		cout << "Invalid configuration parameters. Check config file." << endl << endl;
		usage(EXIT_FAILURE);
	}
	
	// Display the configuration
	config.display();
	cout << endl;
	
	int num = 1;
	while(1){	
		cout << "FETCH/PARSE #" << num << " --------------------------------------------" << endl;
		
		// Generate fetch_queue  ----------------------------
		string line, sites_filename = config.getValue("SITE_FILE");
		ifstream sites (sites_filename);
		if (sites.is_open()){
			unique_lock<mutex> flck(fetch_queue_mutex);
			while (getline(sites,line)){
				cout << "Thread m: " << "\tfetch_push\t" << line << endl;
				fetch_queue.push(line);
			}
			flck.unlock();
			sites.close();
		} else {
			cout << "Error: Unable to open file "	// I/O error checking
				<< sites_filename << "." << endl;
			exit(EXIT_FAILURE);
		}
		
		std::cout << "[Beginning fetch threads]" << endl;
		// Create fetch threads  ----------------------------
		thread* fetch_threads = new thread[fetch];
		for (int i=0; i<fetch; ++i)
			fetch_threads[i] = thread(_fetch, i);
			
		stop_fetching(); // use condition variable to stop fetching
					
		for (int i=0; i<fetch; ++i) fetch_threads[i].join();
		cout << "[All fetch threads joined]" << endl;
		delete[] fetch_threads;
		
		// Create list of search terms  ----------------------
		vector<pair<string,int> > queries;
		string search_filename = config.getValue("SEARCH_FILE");
		ifstream search (search_filename);
		if (search.is_open()){
			while (getline(search,line)){
				queries.push_back(pair<string,int>(line,0));
			}
			search.close();
		} else {
			cout << "Error: Unable to open file "	// I/O error checking
				<< search_filename << "." << endl;
			exit(EXIT_FAILURE);
		}
		
		std::cout << "[Beginning parse threads]" << endl;
		// Create parse threads  ----------------------------
		thread* parse_threads = new thread[parse];
		for (int i=0; i<parse; ++i)
			parse_threads[i] = thread(_parse, i, queries);
		
		stop_parsing(); // use condition variable to stop parsing
			
		for (int i=0; i<parse; ++i) parse_threads[i].join();
		cout << "[All parse threads joined]" << endl;
		delete[] parse_threads;
		
		// Output results of fetch/parse to file
		output_to_file(to_string(num++) + ".csv");
		
		// Wait
		this_thread::sleep_for(chrono::seconds(period));
	}
	
	exit(EXIT_SUCCESS);
}