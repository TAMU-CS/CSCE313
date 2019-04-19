#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <regex>
#include <fstream>
using namespace std;

int main(){

	int fds[2];
	int fds2[2];
	pipe(fds);
	pipe(fds2);

    int stdout = dup(1);
    int stdin = dup(0);	
	// dup2(fds[1], 1);
	// dup2(fds[0], 0);

	// implementing ls -ls>foo.txt
	// int fd = open("output.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	// dup2(fd, 1); //overwrites stdout with new file
	// execlp("ls", "ls", "-l", "-a", nullptr);

	// implementing grep .txt < foo.txt
	// int fd = open("output.txt", O_RDONLY);
	// dup2(fd, 0);
	// execlp("grep", "grep", ".txt", nullptr);

	// testing printing out with pipes
	//int fd = dup(1);
	//write(fd, "TEST", 5);

	// shell piping
	int pid = fork();
	if(!pid){
		dup2(fds[1], 1);
		close(fds[1]);
		close(fds[0]);
		execlp("ls", "ls", "-l", NULL);
	}

	wait(NULL);

	cout << "ls done..." << endl;
	pid = fork();

	if(!pid){
		cout << "child: " << getpid() << endl;
		dup2(fds[0], 0);
		dup2(fds2[1], 1);
		close(fds[0]);
		close(fds[1]);
		close(fds2[1]);
		execlp("./testFunc", "./testFunc", NULL);
	}else{
		cout << "wait for child begin..." << pid << endl;
		close(fds[0]);
		close(fds[1]);
		waitpid(pid, NULL, WUNTRACED);
		cout << "wait for child end." << endl;

		//char buffer[100];
		//read(fds2[0], buffer, 100);
		//cout << buffer << endl;

		dup2(fds2[0], 0);
		close(fds2[1]);
		close(fds2[0]);
		string temp;
		while(getline(cin , temp)){
			cout << temp << endl;
		}
	}

	return 0;
}
