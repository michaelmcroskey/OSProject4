#include <iostream>		// for cout
#include <fstream>		// for ifstream
#include <string>		// for string
#include <queue>			// for sites

using namespace std;

class SitesFile {
	public:                   
		SitesFile(string sites_filename);
		~SitesFile();
		
		string top();
		void pop();
		void display();
	private:
		queue<string> websites;
};

SitesFile::SitesFile(string sites_filename){
	string line;
	ifstream sites (sites_filename);
	if (sites.is_open()){
		while (getline(sites,line)){
			websites.push(line);
		}
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
	while(!websites.empty()){
		cout << websites.front() << endl;
		websites.pop();
	}
	return;
}

string SitesFile::top(void){
	return websites.front();
}

void SitesFile::pop(void){
	websites.pop();
}
