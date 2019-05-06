#include "common.h"
#include "SHMreqchannel.h"
using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side, int _bs) :
	RequestChannel(_name, _side, _bs)
{
	string pipe1 = "shared_" + my_name + "1";
	string pipe2 = "shared_" + my_name + "2";

	if(_side == CLIENT_SIDE){
		clBuff = new SHMbb(pipe1, _bs);
		servBuff = new SHMbb(pipe2, _bs);
	}else{
		clBuff = new SHMbb(pipe2, _bs);
		servBuff = new SHMbb(pipe1, _bs);
	}

}

SHMRequestChannel::~SHMRequestChannel()
{
	delete servBuff;
	delete clBuff;
}

char* SHMRequestChannel::cread(int *len)
{
	if(len){
		*len = buffersize;
	}

	return clBuff->pop();
}

int SHMRequestChannel::cwrite(char* msg, int len)
{
	servBuff->push(msg, len);

	return len;
}