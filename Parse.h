#include <iostream>		// for cout
#include <fstream>		// for ifstream
#include <string>		// for string
#include <queue>			// for queries
#include <mutex>			// for mutex

using namespace std;

class Parse {
	public:                   
		Parse();
		~Parse();
		
		string top();
		void pop();
		void push(string html);
		void display();
	private:
		queue<string> raw_html;
		mutex m;
};

Parse::Parse(){
	;
}

Parse::~Parse(void){
	while(!raw_html.empty()) raw_html.pop();
}

void Parse::display(void){
	lock_guard<mutex> guard(m);
	while(!raw_html.empty()){
		cout << raw_html.front() << endl;
		raw_html.pop();
	}
	return;
}

string Parse::top(void){
	lock_guard<mutex> guard(m);
	return raw_html.front();
}

void Parse::pop(void){
	lock_guard<mutex> guard(m);
	raw_html.pop();
}

void Parse::push(string html){
	lock_guard<mutex> guard(m);
	raw_html.push(html);
}