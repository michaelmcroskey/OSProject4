#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
	
	string page = "<!doctype html><html><body>Terms and Conditions. See our Terms</body></html>";
	vector<string> queries;
	queries.push_back("Terms");
	queries.push_back("Conditions");
	
	for (auto query : queries){
		int count = 0;
		size_t pos = page.find(query, 0); // fist occurrence
		while(pos != string::npos){
			count++;
			pos = page.find(query, pos+1);
		}
		cout << query << "::" << count << endl;
	}
	
}