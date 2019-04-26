#ifndef _RequestChannel_H_
#define _RequestChannel_H_

#include "common.h"
using namespace std;

//abstract class definition of request channel
class RequestChannel
{
public:
	enum Side {SERVER_SIDE, CLIENT_SIDE};
	enum Mode {READ_MODE, WRITE_MODE};
	
	string my_name;
	Side my_side;
	int buffersize;
	
    //pure virtual function definitions
    RequestChannel(){};
	RequestChannel(const string _name, const Side _side, int _bs): 
        my_name( _name), my_side(_side), buffersize(_bs){};

	virtual ~RequestChannel(){};

	virtual char* cread(int *len=NULL) = 0;
	virtual int cwrite(char *msg, int msglen) = 0;

    string name(){return my_name;}
};

#endif