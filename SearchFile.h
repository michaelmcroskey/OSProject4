#include <iostream>		// for cout
#include <fstream>		// for ifstream
#include <string>		// for string
#include <queue>			// for queries

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
	while(!queries.empty()){
		cout << queries.front() << endl;
		queries.pop();
	}
	return;
}

string SearchFile::top(void){
	return queries.front();
}

void SearchFile::pop(void){
	queries.pop();
}