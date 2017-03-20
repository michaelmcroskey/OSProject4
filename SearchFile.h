#include <iostream>		// for cout
#include <fstream>		// for ifstream
#include <string>		// for string
#include <queue>			// for queries
#include <mutex>			// for mutex

using namespace std;

class SearchFile {
	public:                   
		SearchFile(string search_filename);
		~SearchFile();
		
		string top();
		void pop();
		void display();
	private:
		queue<string> queries;
		mutex m;
};

SearchFile::SearchFile(string search_filename){
	string line;
	ifstream search (search_filename);
	if (search.is_open()){
		while (getline(search,line)){
			queries.push(line);
		}
		search.close();
	} else {
		cout << "Error: Unable to open file "
			<< search_filename << "." << endl;
		exit(EXIT_FAILURE);
	}
}

SearchFile::~SearchFile(void){
	while(!queries.empty()) queries.pop();
}

void SearchFile::display(void){
	lock_guard<mutex> guard(m);
	while(!queries.empty()){
		cout << queries.front() << endl;
		queries.pop();
	}
	return;
}

string SearchFile::top(void){
	lock_guard<mutex> guard(m);
	return queries.front();
}

void SearchFile::pop(void){
	lock_guard<mutex> guard(m);
	queries.pop();
}