#include <string>
#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
using namespace std;


class patient_thread_args{
public:
   /*put all the arguments for the patient threads */

    
};

class worker_thread_args{
public:
   /*put all the arguments for the wroker threads */
};

void * patient_thread_function(void *arg)
{
    /* What will the patient threads do? */
    
}

void *worker_thread_function(void *arg)
{
    /*
		Functionality of the worker threads	
    */

    
}

//function prototypes
bool getInput(int &n, int &p, int &w, int &b, int &m, string &f, int argc, char **argv);

int main(int argc, char *argv[])
{
    int n = 100;    //default number of requests per "patient"
    int p = 10;     // number of patients [1,15]
    int w = 100;    //default number of worker threads
    int b = 20; 	// default capacity of the request buffer, you should change this default
	int m = MAX_MESSAGE; 	// default capacity of the file buffer
    string f;   //file string
    srand(time_t(NULL));
    
    //process input
    bool errout = false;
    errout = getInput(n, p, w, b, m, f, argc, argv);
    if(errout){
        cout << "Input Error!" << endl;
        return 0;
    }
    
    int pid = fork();
    if (pid == 0){
		// modify this to pass along m
        execl ("dataserver", "dataserver", m,(char *)NULL);
        
    }
    
	FIFORequestChannel* chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
    BoundedBuffer request_buffer(b);
	HistogramCollection hc;
	
	
	
    struct timeval start, end;
    gettimeofday (&start, 0);

    /* Start all threads here */
	

	/* Join all threads here */
    gettimeofday (&end, 0);
	hc.print ();
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micor seconds" << endl;

    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}

//getInput - uses getopt to parse through flags and sets proper variables to the flag
bool getInput(int &n, int &p, int &w, int &b, int &m, string &f, int argc, char **argv)
{
	int c;
	opterr = 0;

	//get option retrieve user input
	while ((c = getopt(argc, argv, "hn:p:w:b:m:f:")) != -1)
	{
		switch (c)
		{
		case 'n': //number of data items
			if (optarg)	n = atoi(optarg);
			break;
		case 'p': //number of patients
			if (optarg)	p = atoi(optarg);
			break;
		case 'w': //number of worker threads
			if (optarg)	w = atoi(optarg);
			break;
		case 'b': //bounded buffer size
			if (optarg)	b = atoi(optarg);
			break;
		case 'm': //max buffer size
			if (optarg)	m = atoi(optarg);
			break;            
		case 'f': //file name
			if (optarg)	f = optarg;
			break;
        case 'h': //ask for help
			cout << "HELP:\n";
				//  << "-b arg1 : Sets the blocksize to arg1 in bytes (defaults to 128 bytes)\n"
				//  << "-s arg1 : Sets the memory size to arg1 in bytes (defaults 512 kB)\n\n";
			return true;
			break;
		default:
			// if (optopt == 's')
			// {
			// 	cout << "Option -s requires an argument\n";
			// }
			// if (optopt == 'b')
			// {
			// 	cout << "Option -b requires an argument\n";
			// }
			return true;
		}
	}
}
