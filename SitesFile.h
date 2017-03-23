#include <iostream>		// for cout
#include <fstream>		// for ifstream
#include <string>		// for string
#include <queue>			// for sites
#include <mutex>			// for mutex

using namespace std;

class SitesFile {
	public:                   
		SitesFile(string sites_filename);
		~SitesFile();
		
		string top();
		void pop();
		void display();
		
		bool are_more_urls();
		
	private:
		queue<string> websites;
		mutex m;
		bool more_urls;
};

SitesFile::SitesFile(string sites_filename){
	string line;
	ifstream sites (sites_filename);
	if (sites.is_open()){
		while (getline(sites,line)){
			websites.push(line);
		}
		more_urls = true;
		sites.close();
	} else {
		cout << "Error: Unable to open file "
			<< sites_filename << "." << endl;
		exit(EXIT_FAILURE);
	}
}

SitesFile::~SitesFile(void){
	while(!websites.empty()) websites.pop();
}

void SitesFile::display(void){
	lock_guard<mutex> guard(m);
	while(!websites.empty()){
		cout << websites.front() << endl;
		websites.pop();
	}
	return;
}

string SitesFile::top(void){
	lock_guard<mutex> guard(m);
	string url = websites.front();
	websites.pop();
	if (websites.empty()){
		more_urls = false;
	}
	return url;
}

void SitesFile::pop(void){
	lock_guard<mutex> guard(m);
	websites.pop();
}

bool SitesFile::are_more_urls(){
	lock_guard<mutex> guard(m);
	return more_urls;
}