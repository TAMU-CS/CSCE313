#include "common.h"
#include "SHMreqchannel.h"
using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side, int _bs):
	RequestChannel(_name, _side, _bs)
{
	
	pipe1 = "fifo_" + my_name + "1";
	pipe2 = "fifo_" + my_name + "2";
		
	if (_side == SERVER_SIDE){
		wfd = open_pipe(pipe1, O_WRONLY);
		rfd = open_pipe(pipe2, O_RDONLY);
	}
	else{
		rfd = open_pipe(pipe1, O_RDONLY);
		wfd = open_pipe(pipe2, O_WRONLY);
		
	}
	
}

SHMRequestChannel::~SHMRequestChannel()
{ 
	close(wfd);
	close(rfd);

	remove(pipe1.c_str());
	remove(pipe2.c_str());
}

int SHMRequestChannel::open_pipe(string _pipe_name, int mode)
{
	mkfifo (_pipe_name.c_str (), 0600);
	int fd = open(_pipe_name.c_str(), mode);
	if (fd < 0){
		EXITONERROR(_pipe_name);
	}
	return fd;
}

char* SHMRequestChannel::cread(int *len)
{
	char * buf = new char [MAX_MESSAGE];
	int length; 
	length = read(rfd, buf, MAX_MESSAGE);
	if (len)	// the caller wants to know the length
		*len = length;
	return buf;
}

int SHMRequestChannel::cwrite(char* msg, int len)
{
	if (len > MAX_MESSAGE){
		EXITONERROR("cwrite");
	}
	if (write(wfd, msg, len) < 0){
		EXITONERROR("cwrite");
	}
	return len;
}