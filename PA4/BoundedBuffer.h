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
	pthread_cond_t prod_done, cons_done; //condition variables keep track of events that happen
public:
	BoundedBuffer(int _cap){
		//set the queue cap
		cap = _cap;

		//inititialize the mutex, it is unlocked
		pthread_mutex_init(&mut, NULL);

	}
	~BoundedBuffer(){
		//release the mutex
		pthread_mutex_destroy(&mut);
	}

	void push(char* data, int len){
		//populate vector data
		vector<char> t(data, data + len);

		//safely push
		pthread_mutex_lock(&mut); //lock mutex before accessing shared variables

		//wait until consumer is done if too full
		while(q.size() >= cap){
			pthread_cond_wait(&cons_done, &mut);
		}

		//push
		q.push(t);

		//send signal to consumers that producer thread is done
		pthread_cond_signal(&prod_done);

		pthread_mutex_unlock(&mut); //unlock to release access
	}

	vector<char> pop(){

		//thread safe popping
		pthread_mutex_lock(&mut); //lock mutex before accessing shared variables

		//ensure that the queue is not empty;
		while(q.size() <= 0){
			pthread_cond_wait(&prod_done, &mut);
		}

		//pop
		vector<char> t = q.front();
		q.pop();

		//send signal to producers
		pthread_cond_signal(&cons_done);

		pthread_mutex_unlock(&mut); //unlock to release access

		return t;  
	}
};

#endif /* BoundedBuffer_ */
