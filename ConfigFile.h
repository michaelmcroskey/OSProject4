#include <iostream>		// for cout
#include <fstream>		// for ifstream
#include <string>		// for string
#include <map>			// for map
#include <mutex>			// for mutex

using namespace std;

// UTILITY FUNCTIONS -------------------------------------

// String splitting
template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim))
		*(result++) = item;
}
// String splitting
vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

//---------------------------------------------------------

class ConfigFile {
	public:                   
		ConfigFile(string config_filename);
		~ConfigFile();
		
		void display();
		string getValue(string key);
	private:
		map<string, string> config_vars;
		void parse_config_vars(string line);
		void init_map();
		mutex m;
};

ConfigFile::ConfigFile(string config_filename){
	
	init_map();
	
	string line;
	ifstream config (config_filename);
	if (config.is_open()){
		while (getline(config,line)){
			parse_config_vars(line);
		}
		config.close();
	} else {
		cout << "Error: Unable to open file "
			<< config_filename << "." << endl;
		exit(EXIT_FAILURE);
	}
}

ConfigFile::~ConfigFile(void){
	config_vars.clear();
}

void ConfigFile::display(void){
	for (auto& attribute : config_vars) {
		cout << attribute.first << "=" << attribute.second << endl;
	}
	return;
}

string ConfigFile::getValue(string key){
	lock_guard<mutex> guard(m);
	return config_vars[key];
}

// Initialize map to default values
void ConfigFile::init_map(){
	// Define constants
	string PERIOD_FETCH = "180";
	string NUM_FETCH = "1";
	string NUM_PARSE = "1";
	string SEARCH_FILE = "";
	string SITE_FILE = "";
	
	config_vars["PERIOD_FETCH"] = PERIOD_FETCH;
	config_vars["NUM_FETCH"] = NUM_FETCH;
	config_vars["NUM_PARSE"] = NUM_PARSE;
	config_vars["SEARCH_FILE"] = SEARCH_FILE;
	config_vars["SITE_FILE"] = SITE_FILE;
	
	return;
}

// Create map of configuration variables
void ConfigFile::parse_config_vars(string line){
	vector<string> args = split(line, '=');
	string key = args[0];
	string value = args[1];
	if (config_vars.find(key) == config_vars.end()){
		cout << "Warning: unknown parameter " << key << "." << endl;
	} else {
		config_vars[key] = value;
	}	
	return;
}