
#ifndef _SHMreqchannel_H_
#define _SHMreqchannel_H_

#include "common.h"
#include "RequestChannel.h"

class SHMRequestChannel : public RequestChannel
{	
private:
	/*  The current implementation uses named pipes. */
	int wfd;
	int rfd;
	
	string pipe1, pipe2;
	int open_pipe(string _pipe_name, int mode);

	
public:
	SHMRequestChannel(const string _name, const Side _side, int _bs);

	~SHMRequestChannel();

	char* cread(int *len=NULL);

	int cwrite(char *msg, int msglen);
};

#endif
