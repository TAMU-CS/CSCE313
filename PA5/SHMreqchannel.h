
#ifndef _SHMreqchannel_H_
#define _SHMreqchannel_H_

#include "common.h"
#include "RequestChannel.h"

//helper class for the request channel
class SHMbb {
public:
	KernelSemaphore *full, *empty;
	char *data;
	string name;
	int fd;
	int id;
	int length;
	int maxlength;

	SHMbb(string n, int _maxlength){
		name = "/" + n;
		full = new KernelSemaphore(n + "f", 0);
		empty = new KernelSemaphore(n + "e", 1);
		maxlength = _maxlength;

		fd = shm_open(name.c_str(),O_RDWR|O_CREAT, 0644);
		ftruncate(fd, maxlength);
		data = (char*) mmap(NULL, maxlength, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		if(data < 0){
			perror("shm_open");
			exit(0);
		}
	};
	~SHMbb(){
		delete full;
		delete empty;

		close(fd);
		munmap((void*)data, maxlength);
		shm_unlink(name.c_str());
	};
	void push(char* msg, int _length){
		empty->P();
		memcpy(data, msg, _length);
		full->V();
	}
	char* pop(){
		full->P();
		char* msg = new char[maxlength];
		memcpy(msg, data, maxlength);
		empty->V();

		return msg;
	}

};

class SHMRequestChannel : public RequestChannel
{	
private:
	SHMbb *servBuff;
	SHMbb *clBuff;
	
public:
	SHMRequestChannel(const string _name, const Side _side, int _bs);

	~SHMRequestChannel();

	char* cread(int *len=NULL);

	int cwrite(char *msg, int msglen);
};

#endif
