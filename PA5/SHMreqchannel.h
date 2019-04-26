
#ifndef _SHMreqchannel_H_
#define _SHMreqchannel_H_

#include "common.h"
#include "RequestChannel.h"

//helper class for the request channel
class SHMbb {
public:
	KernelSemaphore* full, *empty;
	char *data;
	string name;
	int id;

	SHMbb(string n){

	};
	~SHMbb(){

	};
	void push(string msg){

	}
	string pop(){

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
