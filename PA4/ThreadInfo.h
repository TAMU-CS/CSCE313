#include <string>
#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
using namespace std;

//global variable to keep track of time
int *totalFileLength;
int *completedFileLength = new int(0);
HistogramCollection *global_hc;

class TimerClass {
    public:
    //constructor
    TimerClass(bool _rt){
        this->rt = _rt;
        // Define the timer specification
        // One second till first occurrence
        this->timerSpecs.it_value.tv_sec = 0;
        this->timerSpecs.it_value.tv_nsec = 100000;
        // and then all 3 seconds a timer alarm
        this->timerSpecs.it_interval.tv_sec = 1;
        this->timerSpecs.it_interval.tv_nsec = 0;
        
        // Clear the sa_mask
        sigemptyset(&this->SignalAction.sa_mask);
        // set the SA_SIGINFO flag to use the extended signal-handler function
        this->SignalAction.sa_flags = SA_SIGINFO;
        
        // Define sigaction method
        // This function will be called by the signal
        this->SignalAction.sa_sigaction = TimerClass::alarmFunction;
        
        // Define sigEvent
        // This information will be forwarded to the signal-handler function
        memset(&this->signalEvent, 0, sizeof(this->signalEvent));
        // With the SIGEV_SIGNAL flag we say that there is sigev_value
        this->signalEvent.sigev_notify = SIGEV_SIGNAL;
        // Now it's possible to give a pointer to the object
        this->signalEvent.sigev_value.sival_ptr = (void*) this;
        // Declare this signal as Alarm Signal
        this->signalEvent.sigev_signo = SIGALRM;
        
        // Install the Timer
        if (timer_create(CLOCK_REALTIME, &this->signalEvent, &this->timerID)
        != 0) { // timer id koennte mit private probleme geben
        perror("Could not creat the timer");
        exit(1);
        }
        
        // Finally install tic as signal handler
        if (sigaction(SIGALRM, &this->SignalAction, NULL)) {
        perror("Could not install new signal handler");
        }
    }
    
    void start() {
        // Set the timer and therefore it starts...
        if (timer_settime(this->timerID, 0, &this->timerSpecs, NULL) == -1) {
            perror("Could not start timer:");
        }
    }
    
    /**
    * The signal handler function with extended signature
    */
    static void alarmFunction(int sigNumb, siginfo_t *si, void *uc) {
        // get the pointer out of the siginfo structure and asign it to a new pointer variable
        TimerClass * ptrTimerClass =
        reinterpret_cast<TimerClass *> (si->si_value.sival_ptr);
        // call the member function
        ptrTimerClass->memberAlarmFunction();
    }
    
    // Stored timer ID for alarm
    timer_t timerID;
    sigset_t SigBlockSet;
    struct sigevent signalEvent;
    struct sigaction SignalAction;
    struct itimerspec timerSpecs;
    
    private:
    bool rt;
        
    void memberAlarmFunction() {
        if(rt){
            //display for regular datarequests
            system("clear");
            global_hc->print();
        }else{
            //display for file requests
            system("clear");
            cout << *completedFileLength << "/" << *totalFileLength << endl;
            
            //display = or space based on info
            int barlength = 15;
            int ratio =  (float)*completedFileLength/(float)*totalFileLength * (float)barlength;
            cout << "[";
            for(int i = 0; i < ratio; i++){
                cout << "=";
            }
            for(int i = ratio; i < barlength; i++){
                cout << " ";
            }
            cout << "]\n";

        }
    }
};

//requesting data points
class patient_thread_args{
public:
	//patient thread arguments
	BoundedBuffer *B;
	int n; //number of datapoints
	int p; //patient number
};

class worker_thread_args{
public:
	//worker thread arguments
	BoundedBuffer *B;
	FIFORequestChannel *chan;
	HistogramCollection *H;
};

void *patient_thread_function(void *arg)
{
	//patient threads must do:
	//1. generate datamsgs based on number of requests specified by user (-n flag)
	//2. stores them in the request buffer

	//get arguments
	patient_thread_args *ta = (patient_thread_args*) arg;
	int pNum = ta->p;
	int n = ta->n;

    //datamsg requesting for each datapoint
    for(int i = 0; i < n; i++){
        //send a request for information (person, seconds, ecg number)
	    datamsg *request = new datamsg(1, 0, 1);
        *request = datamsg(pNum, (double)i * 0.004, 1);

		//push onto bounded buffer for worker thread to handle
		ta->B->push((char*) request, sizeof(datamsg));
    }
    //create output file
}

void *worker_thread_function(void *arg)
{
	//worker thread performs four tasks:
	//1. reads a datamsg from request buffer
	//2. sends it to a server over a data channel
	//3. receives the response from server through a data channel
	//4. puts the response in the patient's histogram

	//get arguments
	worker_thread_args *ta = (worker_thread_args *) arg;
	FIFORequestChannel *customChannel = ta->chan;

	datamsg *request = new datamsg(1, 0, 1);

	int *len = new int(100);
	char *buffer = new char(100);

	datamsg *reqDMsg = new datamsg(1,0,1);

	//pop arguments from request buffer
	while(true){
 
		//pop off request and process
		vector<char> vReq = ta->B->pop();
	
		//person and seconds and get reqDMsg conversion
		request = (datamsg*)&vReq[0];
		int person = request->person;
		double seconds = request->seconds;

		//check if quit msg, then stop thread
		if(request->mtype == QUIT_MSG){
			//close channel
   	 		customChannel->cwrite ((char*)request, sizeof (datamsg));
			break;
		}

		//send a request for information (person, seconds, ecg number)
		//ecg = 1
		*request = datamsg(person, seconds, 1);
		customChannel->cwrite((char*)request, sizeof(datamsg));
		buffer = customChannel->cread(len);
		double resp1 = *((double*)buffer);

		*request = datamsg(person, seconds, 2);
		customChannel->cwrite((char*)request, sizeof(datamsg));
		buffer = customChannel->cread(len);
		double resp2 = *((double*)buffer);

		//fill histogram with information
		ta->H->update(person, resp1);
	}
}

//requesting files
class file_threads_args{
	public:
	BoundedBuffer *B;
	FIFORequestChannel *chan;
	char * fileName;
	int m;
};

class file_worker_args{
	public:
	BoundedBuffer *B;
	FIFORequestChannel *chan;
	//output file
	char *fileName;
};

void *file_threads_func(void *arg){

	//get arguments
	file_threads_args *ta = (file_threads_args*) arg;
	int m = ta->m;
	FIFORequestChannel *chan = ta->chan;
	char *fileName = "mybin";
	BoundedBuffer *B = ta->B;

	//get file size
	filemsg *fmsg = new filemsg(0, 0);
	int requestSize = sizeof(filemsg) + sizeof(char) * 5 + 1;
    
    char * request = new char[requestSize];
    *((filemsg *) request) = *fmsg;

    strcpy(request + sizeof(filemsg), fileName);
    chan->cwrite(request, requestSize);

    int * resultLength = new int(0);
    int size = *((int*)chan->cread(resultLength));
    totalFileLength = &size;

    //start timer
    TimerClass timer(false);
    timer.start();

    //send a request for information (person, seconds, ecg number)
    ((filemsg *) request)->length = m;
    ((filemsg *) request)->offset = 0;

    //loop and request
    char *buffer;
    for(int i = m; i < size; i+= m){
        ((filemsg *) request)->offset = i - m;

        //write to bounded buffer
		B->push(request, requestSize);

        *completedFileLength = i;
    }

	if(size % m != 0){

		//set the request length to the ending
		((filemsg *) request)->length = size % m;
		((filemsg *) request)->offset = size  - (size % m);
	
        //write to bounded buffer
		B->push(request, requestSize);
	}
}

void *file_worker_func(void *arg){
	//worker thread performs four tasks:
	//1. reads a datamsg from request buffer
	//2. sends it to a server over a data channel
	//3. receives the response from server through a data channel
	//4. puts the response in the patient's histogram

	//get arguments
	file_worker_args *ta = (file_worker_args *) arg;
	FIFORequestChannel *chan = ta->chan;
	char *fileName = "mybin";//ta->fileName;

	int *len = new int(100);
	char *buffer = new char(100);
	int requestSize = sizeof(filemsg) + sizeof(char) * 5 + 1;
    int * resultLength = new int(0);
    char * request = new char[requestSize];

    //file setup
	FILE* y1output = fopen ("BinCpyOutput", "wb");

	//pop arguments from request buffer
	while(true){
 
		//pop off request and process
		vector<char> vReq = ta->B->pop();
	
		//person and seconds and get reqDMsg conversion
		request = &vReq[0];

		//check if quit msg, then stop thread
		if(((datamsg*)request)->mtype == QUIT_MSG){
			//close channel
   	 		chan->cwrite ((char*)request, sizeof (datamsg));
			break;
		}

        chan->cwrite(request, requestSize);
        buffer = chan->cread(resultLength);

		fseek (y1output, ((filemsg *) request)->offset, SEEK_SET);
		fwrite (buffer, 1, ((filemsg *) request)->length, y1output);
	}
}
