#include "common.h"
#include "SHMreqchannel.h"
using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side, int _bs):
	RequestChannel(_name, _side, _bs)
{
	
	string pipe1 = "shared_" + my_name + "1";
	string pipe2 = "shared_" + my_name + "2";
	servBuff = new SHMbb(pipe1);
	clBuff = new SHMbb(pipe2);

}

SHMRequestChannel::~SHMRequestChannel()
{
	delete servBuff;
	delete clBuff;
}

char* SHMRequestChannel::cread(int *len)
{
	if(my_side == RequestChannel::SERVER_SIDE){
		return (char*)servBuff->pop().c_str();
	}else{
		return (char*)clBuff->pop().c_str();
	}
}

int SHMRequestChannel::cwrite(char* msg, int len)
{
	if(my_side == RequestChannel::SERVER_SIDE){
		clBuff->push(msg);
	}else{
		servBuff->push(msg);
	}

	return len;
}