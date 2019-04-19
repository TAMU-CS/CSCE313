/*
    Title: Linux Shell
    Last problem:

    Completed features:
*/

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <stdio.h>
#include <ctype.h>

using namespace std;

//global jobs variable
unordered_map<int, vector<string> > jobs;

//functions for evaluating input
bool parseInput(vector<string> &tokens);
bool eval(string command, vector<string> args);
bool evalSpecial(string command, vector<string> args);
bool is_number(string& s);
void updateJobs();

int main(int argc, char **argv){

    //std pipes
    int stdout = dup(1);
    int stdin = dup(0);

    //give terminal information
    cout << "Terminal created by Kevin Tang\n\n";
    cout << getcwd(nullptr, 0) << ": ";
    string input;

    //loop until user exits the shell with input exit
    while(getline(cin, input)){
        //tokenize the string , get vector of strings
        // regex r = regex("\\s+"); //tokenize by matches to regex
        // regex_token_iterator<string::iterator> rit(input.begin(), input.end(), r, -1); //split using regex iterator
        // regex_token_iterator<string::iterator> rend;
        // vector<string> tokens(rit, rend);
        
        // // stringstream ss(input);
        // // vector<string> tokens;
        // // string temp;
        // // while (ss >> quoted(temp, '\'')){
        // //     tokens.push_back(temp);
        // // }

        //push space at end of input string to help parsing conventions (it terminates at space, not end of string)
        input.push_back(' ');

        //iterate through input and create a vector of tokens
        vector<string> tokens;
        for(int i = 0; i < input.size(); i++){
            //start of new word
            string word;

            //ignore initial white space
            int j = i;
            while(isspace(input[j])){
                j++;
            }
            i = j; //start of word

            //2 cases:
            //1. j is now a quote \"
            //2. j is a regular letter
            //build word
            if(input[j] == '\'' || input[j] == '\"'){ //find end of quote
                char endFlag = input[j];
                j++;
                bool endfound = false;
                while(j < input.size()){
                    if(input[j] == endFlag && input[j - 1] != '\\'){ //make sure its not a \" , but just a '
                        endfound = true;
                        break;
                    }
                    j++;
                }

                //quote is subset of input from i to j
                if(endfound){
                    word = input.substr(i + 1, j - i - 1); //shave out start and end quotes
                    tokens.push_back(word);
                }else{
                    //print error, quotes are not closed
                    cout << "SYNTAX ERROR: quotes are open!" << endl;
                    return 0;
                }
            }else{ //regular character
                while(j < input.size()){ //find next white space or pipe or redirection
                    if(isspace(input[j])){       
                        //create the word
                        word = input.substr(i, j - i);
                        tokens.push_back(word);
                        break;

                    }else if(input[j] == '<' || input[j] == '>' || input[j] == '|'){ //add new word but also add extra words
                        //create seperate words
                        if(j-i > 0){
                            word = input.substr(i, j - i); //previous word
                            tokens.push_back(word);
                        }

                        //add the current character as its own word
                        word = input.substr(j, 1);
                        tokens.push_back(word);

                        break;
                    }
                    j++;
                }
            }

            //set i to later value of j
            i = j;

        }

        //debugging print of token parsing
        // for(auto it = tokens.begin(); tokens.end() != it; ++it){
        //     cout << (*it) << endl;
        // }

        //parse through the input and make sure its valid
        if(tokens.size() != 0 && parseInput(tokens)){
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

    int stdout = dup(1);
    int stdin = dup(0);
    //call update jobs
    updateJobs();
    // int temp[2];
    // pipe(temp);
    // dup2(temp[0], 0);
    // close(temp[1]);
    // dup2(stdin, 0);

    //get sets of tokens divided by pipes
    vector< vector<string> > sets;
    int i = 0; 
    while(true){
        vector<string> set; //single set
        while(i < tokens.size() && tokens[i] != "|"){
            set.push_back(tokens[i]);
            i++;
        }
        if(set.size() > 0){
            sets.push_back(set);
        }

        i++;
        if(i >= tokens.size()){
            break;
        }
    }

    //check to see if we run as background
    bool ischildbackground = false;
    int backgroundpid = -1;
    if(sets[sets.size()-1][sets[sets.size()-1].size() - 1] == "&"){
        //run everything in background
        backgroundpid = fork();
        if(!backgroundpid){
            ischildbackground = true;

            //remove from sets
            sets[sets.size() - 1].erase(sets[sets.size()-1].begin()+sets[sets.size() - 1].size() - 1);
        }else{ //parent, immidiate abort and return
            //add child to process list
            jobs[backgroundpid] = tokens;

            //ignore background
            signal(SIGCHLD,SIG_IGN); 
            return false;
        }
    }

    int fds[2];
    for(int i = 0; i < sets.size(); i++){
        //check for exit command
        if(sets[i][0] == "exit"){
            return true;
        }

        //cout << "working on set: " << i << endl;
        pipe(fds); //connecting pipe between previous set of commands

        //determine redirection files
        //erase out of set for argument lists
        vector<string> args;
        int inpipe = -1;
        int outpipe = -1;
        for(int j = 0; j < sets[i].size(); j++){

            if(sets[i][j] == "<"){
                //open file buffer and set it as input
                if(j == sets[i].size() - 1){
                    cout << "SYNTAX ERROR: invalid redirection use\n";
                    return false;
                }
                
                //setup file stream
                int fd = open(sets[i][j + 1].c_str(), O_RDONLY);

                //overwrite input stream with fd
                inpipe = fd;

                //move to next
                j++;
            }else if(sets[i][j] == ">"){
                //open file buffer asnd set as output
                if(j == sets[i].size() - 1){
                    cout << "SYNTAX ERROR: invald redirection\n";
                    return false;
                }

                //output fd
                int fd = open(sets[i][j + 1].c_str(),  O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

                //overwrite output stream
                outpipe = fd;

                //move to next
                j++;
            }else{
                //add to argument list
                args.push_back(sets[i][j]);
            }

        }

        int pid = fork();

        if(!pid){

            if(sets.size() > 1){
                if(i == sets.size() - 1){
                    dup2(stdout, 1);
                }else{
                    dup2(fds[1], 1);
                }
            }

            //setup file pipes
            if(outpipe != -1){
                dup2(outpipe, 1);
            }
            if(inpipe != 1){
                dup2(inpipe, 0);
            }

            // if(i == 0 && sets.size() > 1){//beginning
            //     //change output to fds out
            //     dup2(fds[1], 1);
            // }else if(sets.size() > 1 && i > 0 && i < sets.size() - 1){ //generic middle pipes
            //     dup2(fds[1], 1);
            // }else if(i == sets.size() - 1 && sets.size() > 1){ //end
                
            // }

            //cout << "inpipe: " << dup(0) << " outpipe: " << dup(1) << endl;
            if(sets[i][0] == "cd" || sets[i][0] == "jobs"){ //exit command
                return !evalSpecial(sets[i][0], args); //if false then exit is called, or unsuccessful, terminate
            }else{ //regular command
                eval(sets[i][0], args); //if unsuccessful then terminate
            }
            
            cout << i << ": didn't work" << endl;
            abort();
        }

        //wait for child to execute
        dup2(fds[0], 0);
        close(fds[1]);

        //cout << i << ": func begin" << endl;
        waitpid(pid, NULL, WUNTRACED);

        // char buffer[100];
        // read(fds[0], buffer, 100);
        // cout << buffer << endl;

        //cout << i << ": func end" << endl;

        //set pipes
        // lastPipe[0] = fds[0];
        // lastPipe[1] = fds[1];
        
    }

    //terminate background child
    if(ischildbackground){
        abort();
    }

    //reset stdin and out
    dup2(stdin, 0);
    dup2(stdout, 1);

    return false;
}

//1. Evaluate special commands
//input is the command, arguments
//returns true if successful , false otherwise or if exit detected
bool evalSpecial(string command, vector<string> args){
    if(command == "exit"){
        abort();
        return false;
    }else if(command == "cd"){
        //change working directory
        if(args[1] == "-"){
            chdir("..");
        }else{
            chdir(args[1].c_str());
        }
        return true;
    }else if(command == "jobs"){

        //print out all the current jobs
        for(auto it = jobs.begin(); it != jobs.end(); ++it){
            cout << "job id: " << it->first << " | ";
            for(int i = 0; i < (it->second.size()); i++){
                cout << " " << it->second[i];
            }
            cout << endl;
        }

    }
}

//2. Evaluate normal commands
//input is the command, and vector of string arguments
//output is true if successful, or false if unsuccessful
bool eval(string command, vector<string> args){


    //convert args vector to const char **
    char *nargs[args.size()];
    for(int i = 0; i < args.size(); i++){
        nargs[i] = (char*)args[i].c_str();
    }
    nargs[args.size()] = nullptr;

    execvp(command.c_str(), nargs);
    cout << "ERROR: Command Does Not Exist- " << command.c_str() << "\n";

    abort();
}

//determines if string is number
bool is_number(string& s)
{
    for(auto it = s.begin(); it != s.end(); ++it){
        if(!isdigit(*it)){
            return false;
        }
    }
    return true;
}

//updated current jobs
void updateJobs(){
    //original in and out
    int stdin = dup(0);
    int stdout = dup(1);

    //remove jobs from the job map
    int fds[2];
    pipe(fds);

    int pid = fork();
    if(!pid){
        dup2(fds[1], 1); //set output
        execlp("ps", "ps", NULL);

        abort();
    }else{
        //get input
        close(fds[1]);

        //reap child
        waitpid(pid, NULL, WUNTRACED);
        
        //read in the input
        stringstream ss;
        string temp;
        char c;
        while (read(fds[0], &c, 1) == 1){
            ss << c;
        }

        //build dictionary
        unordered_map<int, bool> dict;
        while(ss >> temp){
            //remove the temp from the hash map
            if(is_number(temp)){
                int pid = stoi(temp);
                dict[pid] = true;
            }
        }

        //iterate through jobs map and remove ones that are not in service
        auto it = jobs.begin();
        while(it != jobs.end()){
            //remove job if not in processes list
            if(dict.find(it->first) == dict.end()){
                it = jobs.erase(it);
            }else{
                ++it;
            }
        }        
    }

}