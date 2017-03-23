// NOTE: BEFORE SUBMITTING MAKE SURE TO CHANGE COMPILER TO /usr/bin/g++
// -static-libstdc++
// Test on student machines

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

#include "ConfigFile.h"
#include "LibCurl.h"

using namespace std;

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

void handle_exit(int sig) {
	exit(EXIT_SUCCESS);
}

// Mutex and stuff ----------------------------

queue<string> fetch_queue;
mutex fetch_queue_mutex;
condition_variable fetch_cv;

queue<pair<string,string>> parse_queue;
mutex parse_queue_mutex;
condition_variable parse_cv;

vector< vector<string> > results;

void stop_fetching() {
	sleep(15);
	fetch_cv.notify_all();
}

void stop_parsing() {
	sleep(15);
	parse_cv.notify_all();
}

void _fetch(int id) {
	// Keep looping until all urls are processed
	while(1){
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

void _parse(int id, vector<pair<string, int> > queries) {
	while(1){
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

void output_to_file(string output_filename){
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
}

bool no_files(ConfigFile *config){
	ifstream search (config->getValue("SEARCH_FILE"));
	if (!search.is_open()) return true;
	ifstream sites (config->getValue("SITE_FILE"));
	if (!sites.is_open()) return true;
	return false;
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
	
	if (fetch < 0 || parse < 0 || no_files(&config)){
		cout << "Invalid configuration parameters. Check config file." << endl;
		exit(EXIT_FAILURE);
	}
	
	// Display the configuration
	config.display();
	cout << endl;
	
	int num = 1;
	while(1){	
		cout << "FETCH/PARSE #" << num << " -------------------------" << endl;
		
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
			cout << "Error: Unable to open file "
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
			cout << "Error: Unable to open file "
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
		
		output_to_file(to_string(num++) + ".csv");
		
		this_thread::sleep_for(chrono::seconds(period));
	}
	
	exit(EXIT_SUCCESS);
}