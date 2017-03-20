// OS Project 4
// 
// Michael McRoskey
// Maggie Thomann

#include <iostream>
#include <string>

using namespace std;

// Utility Functions --------------------------
void usage(int status) {
	cout << "usage: tool <filename>" << endl;
	exit(status);
}


// Main Execution  ----------------------------
int main(int argc, char *argv[]) {
	
	if (argc != 2){
		usage(EXIT_FAILURE);
	} else {
		string filename = argv[1];
		cout << filename << endl;
	}
}