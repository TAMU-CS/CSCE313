#include "common.h"
#include "MQreqchannel.h"
using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

MQRequestChannel::MQRequestChannel(const string _name, const Side _side, int _bs):
	RequestChannel(_name, _side, _bs)
{
	
	pipe1 = "/mq_" + my_name + "1";
	pipe2 = "/mq_" + my_name + "2";
		
	if (_side == SERVER_SIDE){
		wfd = open_pipe(pipe1, O_WRONLY|O_CREAT);
		rfd = open_pipe(pipe2, O_RDONLY|O_CREAT);
	}
	else{
		rfd = open_pipe(pipe1, O_RDONLY|O_CREAT);
		wfd = open_pipe(pipe2, O_WRONLY|O_CREAT);	
	}
	
}

MQRequestChannel::~MQRequestChannel()
{ 
	//do cleanup
	mq_close(wfd);
	mq_close(rfd);

	mq_unlink(pipe1.c_str());
	mq_unlink(pipe2.c_str());
}

int MQRequestChannel::open_pipe(string _pipe_name, int mode)
{
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 1;
	attr.mq_msgsize = MAX_MESSAGE;
	mqd_t mq = mq_open(_pipe_name.c_str (), mode, 0664, &attr);
	if(mq < 0){
		EXITONERROR(_pipe_name);
	}
	return mq;
}

char* MQRequestChannel::cread(int *len)
{
	struct mq_attr attr;
	mq_getattr(rfd, &attr);
	char *buf = (char*)malloc(attr.mq_msgsize);
	mq_receive(rfd, buf, attr.mq_msgsize, NULL);
	if (len)	// the caller wants to know the length
		*len = attr.mq_msgsize;
	return buf;
}

int MQRequestChannel::cwrite(char* msg, int len)
{
	if (len > MAX_MESSAGE){
		EXITONERROR("cwrite");
	}
	if (mq_send(wfd, msg, len, 0) < 0){
		EXITONERROR("cwrite");
	}
	return len;
}