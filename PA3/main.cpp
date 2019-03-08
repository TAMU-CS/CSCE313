#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>

using namespace std;

bool parseInput(vector<string> &tokens);
bool eval(string command, vector<string> args);
bool evalSpecial(string command, vector<string> args);

int main(int argc, char **argv){

    //give terminal information
    cout << "Terminal created by Kevin Tang\n\n";
    cout << getcwd(nullptr, 0) << ": ";
    string input;

    //loop until user exits the shell with input exit
    while(getline(cin, input)){
        //tokenize the string , get vector of strings
        regex r(R"([\s]+)"); //tokenize by space
        regex_token_iterator<string::iterator> rit(input.begin(), input.end(), r, -1); //split using regex iterator
        regex_token_iterator<string::iterator> rend;
        vector<string> tokens(rit, rend);

        //parse through the input and make sure its valid
        if(parseInput(tokens)){
            return 0; //exit command is detected
        }


        //prefex for next input command
        cout <<  getcwd(nullptr, 0) << ": ";
    }
    return 0;
}

//uses grammar outlined in instructions to parse commands
bool parseInput(vector<string> &tokens){
    /**GRAMMAR
     * S-> UC || UC AMP || SC
     * UC-> command_name ARGS || UC REDIRECTION filename || UC PIPE UC
     * SC-> cd DIRECTORY || exit
     * command_name-> valid executable/interpreted file name
     * AMP-> &
     * ARG-> string
     * ARGs-> ARG ARGS || ARG
     * DIRECTORY-> absolute path || relative path
     * PIPE-> |
     * REDIRECTION-> < || > 
     */

    //check last token and see if its ampersand (run process in background)

    //check for special command
    if(tokens[0] == "exit" || tokens[0] == "cd"){ //exit command
        return !evalSpecial(tokens[0], tokens); //if false then exit is called, or unsuccessful, terminate
    }else{ //regular command
        return !eval(tokens[0], tokens); //if unsuccessful then terminate
    }

    return false;
}

//1. Evaluate special commands
//input is the command, arguments
//returns true if successful , false otherwise or if exit detected
bool evalSpecial(string command, vector<string> args){
    if(command == "exit"){
        return false;
    }else if(command == "cd"){
        //change working directory
        chdir(args[1].c_str());
        return true;
    }
}

//2. Evaluate normal commands
//input is the command, and vector of string arguments
//output is true if successful, or false if unsuccessful
bool eval(string command, vector<string> args){
    
    //run exec and wait till child is done
    int pid = fork();

    if(pid == 0){ //child process
        //convert args vector to const char **
        char *nargs[args.size()];
        for(int i = 0; i < args.size(); i++){
            nargs[i] = (char*)args[i].c_str();
        }
        nargs[args.size()] = nullptr;

        execvp(command.c_str(), nargs);
        cout << "ERROR: Command Does Not Exist!\n";
        return false;
    }else{ //parent process
        waitpid(pid, nullptr, WUNTRACED);
        return true;
    }
}