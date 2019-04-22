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
	queue<vector<char> > q;

	//2 synchronization primitives: a mutex and two condition variables
	pthread_mutex_t mut;
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
