#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <regex>
using namespace std;

int main(){
	string input;
	getline(cin, input);

	//tokenize the string , get vector of strings
	regex r(R"([\s]+)"); //tokenize by space
	regex_token_iterator<string::iterator> rit(input.begin(), input.end(), r, -1); //split using regex iterator
	regex_token_iterator<string::iterator> rend;
	vector<string> tokens(rit, rend);

	//convert vector of strings to c string array
	char *nargs[tokens.size()];
	for(int i = 0; i < tokens.size(); i++){
		nargs[i] = (char*)tokens[i].c_str();
	}
	nargs[tokens.size()] = nullptr;

	//call exec
	execvp(tokens[0].c_str(), nargs);

	cout << "ERROR: execvp failed!" << endl;
	return 0;
}
