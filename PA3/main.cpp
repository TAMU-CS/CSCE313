#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string>

using namespace std;

void parseInput(string input);

int main(int argc, char **argv){

    //give terminal information
    cout << "Terminal created by Kevin Tang\n\n";
    cout << "> ";
    string input;

    //loop until user exits the shell with input exit
    while(getline(cin, input)){
        cout << input << endl;

        //parse through the input and make sure its valid
        parseInput(input);


        //prefex for next input command
        cout << "> ";
    }
    return 0;
}

//uses grammar outlined in instructions to parse commands
void parseInput(string input){
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
     * 
     * Convert GRAMMER to DPDA (deterministic push down automata)
     * Start->q1 - e, e->$
     * q1->q2 - e, e->S
     * q2->q2 - e, S->UC | e, S->UC AMP | e, S->SC
     * 
     */
}
