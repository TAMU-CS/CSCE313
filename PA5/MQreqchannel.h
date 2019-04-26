
#ifndef _MQreqchannel_H_
#define _MQreqchannel_H_

#include "common.h"
#include "RequestChannel.h"

class MQRequestChannel : public RequestChannel
{	
private:
	//Message queues, IPC
	
	int wfd;
	int rfd;
	
	string pipe1, pipe2;
	int open_pipe(string _pipe_name, int mode);

	
public:
	MQRequestChannel(const string _name, const Side _side, int _bs);

	~MQRequestChannel();

	char* cread(int *len=NULL);

	int cwrite(char *msg, int msglen);
};

#endif
