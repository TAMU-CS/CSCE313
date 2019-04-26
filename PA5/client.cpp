
#include "ThreadInfo.h"
#include <signal.h>

//function prototypes
bool getInput(int &n, int &p, int &w, int &b, int &m, string &f, char &i, int argc, char **argv);
void dataReq(int p, int w, int n, RequestChannel* chan, BoundedBuffer *request_buffer, HistogramCollection &hc);
void fileReq(int w, int m, char *fileName, RequestChannel* chan, BoundedBuffer *request_buffer);

int main(int argc, char *argv[])
{
    int n = 100;    //default number of requests per "patient"
    int p = 10;     // number of patients [1,15]
    int w = 100;    //default number of worker threads
    int b = 20; 	// default capacity of the request buffer, you should change this default
	int m = MAX_MESSAGE; 	// default capacity of the file buffer
    string f;   //file string
	Rtype = 'f'; //request channel type , default to fifo
    srand(time_t(NULL));
    
    //process input
    bool errout = false;
    errout = getInput(n, p, w, b, m, f, Rtype, argc, argv);
    if(errout){
        cout << "Input Error!" << endl;
        return 0;
    }

	//determing if filemode
	bool datamode = f == "";

    int pid = fork();
    if (pid == 0){
		// modify this to pass along m
		string s =  to_string(m);
        execl ("./dataserver", "./dataserver", (char*)s.c_str(), &Rtype,(char *)NULL);
        cout << "server failed" << endl;
		return 0;
    }

	//create control request channel
	//make channel based on rtype
	RequestChannel* chan;
	if(Rtype == 'f'){ //fifo
		chan = new FIFORequestChannel("control", RequestChannel::CLIENT_SIDE, m);
	}else if(Rtype == 'q'){ //message queue
		chan = new MQRequestChannel("control", RequestChannel::CLIENT_SIDE, m);
	}else if(Rtype == 's'){ //shared memory
		chan = new SHMRequestChannel("control", RequestChannel::CLIENT_SIDE, m);
	}else{
		cout << "Request Channel must be f,q,s!\n";
		return 0;
	}

    BoundedBuffer *request_buffer = new BoundedBuffer(b);
	HistogramCollection hc;

	//for signal handlers
	global_hc = &hc;
	
    struct timeval start, end;
    gettimeofday (&start, 0);

	if(datamode){
		dataReq(p, w, n, chan, request_buffer, hc);
	}else{
		char *fileName = "mybin";
		fileReq(w, m, fileName, chan, request_buffer);
	}

    gettimeofday (&end, 0);

	//clear before output
    //system("clear");
	if(datamode) hc.print ();
	
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micor seconds" << endl;

    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
	//cleanup
	//not necessary, when channels are deleted they clean themselves
}

void dataReq(int p, int w, int n, RequestChannel* chan, BoundedBuffer *request_buffer, HistogramCollection &hc){


    //keep track of threads and argument lists
	pthread_t tids[p + w];
	vector<worker_thread_args*> workerArgs;
	vector<patient_thread_args*> patientArgs;

	//start all patient threads
	//cout << "starting pthreads: " << p << endl;
	for(int i = 0; i < p; i++){
		//setup patient histogram
		Histogram *tempH = new Histogram(10, -1.0, 1.0);
		hc.add(tempH);

		//setup the patient threads
		patient_thread_args *ta = new patient_thread_args();
		ta->B = request_buffer;
		ta->p = i+1;
		ta->n = n;
		pthread_create(&tids[i], 0, patient_thread_function, ta);
		patientArgs.push_back(ta);
	}

	//start all worker threads
	//cout << "starting wthreads: " << w << endl;
	for(int i = 0; i < w; i++){
		//set up communication channel
		datamsg *request = new datamsg(1, 0, 1);
		request->mtype = NEWCHANNEL_MSG;
		chan->cwrite((char *) request, sizeof(datamsg));

		int *size = new int();
		char *requestedChannel = chan->cread(size);
		RequestChannel *customChannel;
		if(Rtype == 'f'){ //fifo
			customChannel =	new FIFORequestChannel(requestedChannel, RequestChannel::CLIENT_SIDE, chan->buffersize);
		}else if(Rtype == 'q'){ //message queue
			customChannel =	new MQRequestChannel(requestedChannel, RequestChannel::CLIENT_SIDE, chan->buffersize);
		}else if(Rtype == 's'){ //shared memory
			customChannel =	new SHMRequestChannel(requestedChannel, RequestChannel::CLIENT_SIDE, chan->buffersize);
		}

		delete request;
		delete size;
		delete requestedChannel;

		//create the thread
		worker_thread_args *wArgs = new worker_thread_args();
		wArgs->B = request_buffer;
		wArgs->chan = customChannel;
		wArgs->H = &hc;
		pthread_create(&tids[i + p], 0, worker_thread_function, wArgs);
		workerArgs.push_back(wArgs);
	}

	//setup for bonus
	//start timer to send signals and display histogram
	// TimerClass timer(true);
	// timer.start();
	// cout << "timer armed" << endl;

	//join all the patient threads
	for(int i = 0; i < p; i++){
		pthread_join(tids[i], 0);
	}
	//free memory
	for(int i = 0; i < patientArgs.size(); i++) delete patientArgs[i];
	//send stop msg to worker threads
	for(int i = 0; i < w; i++){
		datamsg *request = new datamsg(1, 0, 1);
		request->mtype = QUIT_MSG;
		request_buffer->push((char*)request, sizeof(datamsg));
	}
	//join all worker threads
	for(int i = p; i < p + w; i++){
		pthread_join(tids[i], 0);
	}
	//free memory
	for(int i = 0; i < workerArgs.size(); i++) delete workerArgs[i];

}

void fileReq(int w, int m, char *fileName, RequestChannel* chan, BoundedBuffer *request_buffer){


    //keep track of threads and argument lists
	pthread_t tids[w];
	vector<file_worker_args*> workerArgs;

	//start all worker threads
	//cout << "starting wthreads: " << w << endl;
	for(int i = 0; i < w; i++){
		//set up communication channel
		datamsg *request = new datamsg(1, 0, 1);
		request->mtype = NEWCHANNEL_MSG;
		chan->cwrite((char *) request, sizeof(datamsg));

		int *size = new int();
		char *requestedChannel = chan->cread(size);
		RequestChannel *customChannel;
		if(Rtype == 'f'){ //fifo
			customChannel =	new FIFORequestChannel(requestedChannel, RequestChannel::CLIENT_SIDE, chan->buffersize);
		}else if(Rtype == 'q'){ //message queue
			customChannel =	new MQRequestChannel(requestedChannel, RequestChannel::CLIENT_SIDE, chan->buffersize);
		}else if(Rtype == 's'){ //shared memory
			customChannel =	new SHMRequestChannel(requestedChannel, RequestChannel::CLIENT_SIDE, chan->buffersize);
		}

		delete request;
		delete size;
		delete requestedChannel;

		//create the thread
		file_worker_args *wArgs = new file_worker_args();
		wArgs->B = request_buffer;
		wArgs->chan = customChannel;
		wArgs->fileName = fileName;
		pthread_create(&tids[i], 0, file_worker_func, wArgs);
		workerArgs.push_back(wArgs);
	}
	//join all the file req thread
	//start file thread for pushing to buffer
	pthread_t fTid;
	file_threads_args ta;
	ta.B = request_buffer;
	ta.chan = chan;
	ta.m = m;
	ta.fileName = fileName;
	pthread_create(&fTid, 0, file_threads_func, &ta);

	pthread_join(fTid, 0);
	
	//send stop msg to worker threads
	for(int i = 0; i < w; i++){
		datamsg *request = new datamsg(1, 0, 1);
		request->mtype = QUIT_MSG;
		request_buffer->push((char*)request, sizeof(datamsg));
	}
	//join all worker threads
	for(int i = 0; i < w; i++){
		pthread_join(tids[i], 0);
	}
	//free memory
	for(int i = 0; i < workerArgs.size(); i++) delete workerArgs[i];

}


//getInput - uses getopt to parse through flags and sets proper variables to the flag
bool getInput(int &n, int &p, int &w, int &b, int &m, string &f, char &i, int argc, char **argv)
{
	int c;
	opterr = 0;

	//get option retrieve user input
	while ((c = getopt(argc, argv, "hn:p:w:b:m:f:i:")) != -1)
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
		case 'i': //request channel type
			if (optarg)	i = optarg[0];
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
