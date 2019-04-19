#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <regex>
#include <fstream>
using namespace std;

void func1(){
	execlp("cat", "cat", "output.txt", NULL);
}
void func2(){
	execlp("grep", "grep", ".txt", NULL);
}
void func3(){
	execlp("grep", "grep", "out", NULL);
}

int main(){
    int lastPipe[2];

    //iterate through functions and try to call and pipe
    for(int i = 0; i < 4; i++){
        //setup piping
        int fds[2];
        pipe(fds);
        int pid = fork();

        if(!pid){
            if(i == 0){//beginning
                //change output to fds out
                dup2(fds[1], 1);
                close(fds[1]);
                close(fds[0]);

                func1();
            }else if(i == 1 || i == 2){ //generic middle pipes
                dup2(lastPipe[0], 0);
                dup2(fds[1], 1);
                close(lastPipe[1]);
                close(lastPipe[0]);
                close(fds[1]);
                close(fds[0]);

                func2();
            }else if(i == 3){ //end
                dup2(lastPipe[0], 0);
                close(lastPipe[0]);
                close(lastPipe[1]);
                close(fds[1]);
                close(fds[0]);

                func3();
            }
            cout << i << ": didn't work" << endl;
            abort();
        }

        //wait for child to execute
        if(i >= 1){
            close(lastPipe[0]);
            close(lastPipe[1]);
            //close(fds[0]);
            close(fds[1]);
        }

        cout << i << ": func begin" << endl;
        waitpid(pid, NULL, WUNTRACED);

        // char buffer[100];
        // read(fds[0], buffer, 100);
        // cout << buffer << endl;

        cout << i << ": func end" << endl;

        //set pipes
        lastPipe[0] = fds[0];
        lastPipe[1] = fds[1];

    }
}