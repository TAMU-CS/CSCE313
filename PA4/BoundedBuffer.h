#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>

using namespace std;

class BoundedBuffer
{
private:
  int cap;

public:
	BoundedBuffer(int _cap){

	}
	~BoundedBuffer(){

	}

	void push(char* data, int len){

	}

	vector<char> pop(){
		vector<char> t;
		return t;  
	}
};

#endif /* BoundedBuffer_ */
