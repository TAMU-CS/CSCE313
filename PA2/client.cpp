/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/19
 */
#include "common.h"
#include "FIFOreqchannel.h"

using namespace std;

//1. Requests Data Points: gets all data points in ecg1, ecg2 for person 1 and outputs to file x1.csv
void test1(FIFORequestChannel &chan);

//2. Requesting a Text File: request any file and outputs into y1.csv (default copies BIMDC/1)
void test2(FIFORequestChannel &chan);

//3. Requesting Binary File: treat's the file as binary, copies random binary file of size 2M bytes into BinCpyOutput
void test3(FIFORequestChannel &chan);

//4. Requests a new channel: creates new channel and gets information from BIMDC/1.csv to demonstrate communication
void test4(FIFORequestChannel &chan);

int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));

    //Run all tests and fork server and client
    //Server as child process: uses fork and exec to handle server process, sends quit msg to stop server process
    int cid = fork();

	if(!cid){//child process

        //cout << "Child process:" << getpid() << endl;
		char * arglist[2] = {"./dataserver"};
        arglist[1] = nullptr;
		execvp(arglist[0], arglist);

        return 0; //return at end of child process
	}

    //continue parent process
    //show parent pid
    //cout << "Parent process: " << getpid() << endl;

    //allow server to load
    sleep(1);
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);

    //run tests, measure time for test 1 and test 2
    auto start = chrono::high_resolution_clock::now();
    test1(chan);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "TEST1 took: " << duration.count() << " musec" << endl;

    start = chrono::high_resolution_clock::now();
    test2(chan);
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "TEST2 took: " << duration.count() << " musec" << endl;

    test3(chan);
    test4(chan);

    //quit the server
    datamsg *request = new datamsg(1, 0, 1);
    request->mtype = QUIT_MSG;    
    chan.cwrite((char *)request, sizeof(datamsg));


	return 0;
}

void test1(FIFORequestChannel &chan){
    //1. requesting data points for person 1
    //create output file
    cout << "TEST 1: saved BIMDC/1.csv data request into x1.csv" << endl;
    ofstream person1output("x1.csv");
    int *len = new int(100);
    char *buffer = new char(100);
    datamsg *request = new datamsg(1, 0, 1);

    //keep track of the time
    for(int i = 0; i < 15000; i++){ //15000 different times
        //send a request for information (person, seconds, ecg number)
        *request = datamsg(1, (double)i * 0.004, 1);
        person1output << (double)i * 0.004 << ",";

        chan.cwrite((char*)request, 100);
        buffer = chan.cread(len);
        person1output << *((double*)buffer) << ",";

        *request = datamsg(1, (double)i * 0.004, 2);
        chan.cwrite((char*)request, 100);
        buffer = chan.cread(len);
        person1output << *((double*)buffer) << endl;
    }
    delete len;
    delete request;
    delete buffer;
    person1output.close();

}

void test2(FIFORequestChannel &chan){
    //2. request a test File
    //get the length
    cout << "TEST 2: saved BIMDC/1.csv file request into y1.csv" << endl;
    char * fileName = "1.csv";
    filemsg *fmsg = new filemsg(0, 0);

    //create request
    int requestSize = sizeof(filemsg) + sizeof(char) * 5 + 1;
    char * request = new char[requestSize];
    *((filemsg *) request) = *fmsg;

    strcpy(request + sizeof(filemsg), fileName);
    chan.cwrite(request, requestSize);

    int * resultLength = new int(0);
    int size = *((int*)chan.cread(resultLength));

    //send a request for information (person, seconds, ecg number)
    ((filemsg *) request)->length = 100;
    ((filemsg *) request)->offset = 0;

    //file setup
    ofstream y1output("y1.csv");

    //loop and request
    char *buffer;
    for(int i = 100; i < size; i+= 100){
        ((filemsg *) request)->offset = i - 100;

        chan.cwrite(request,requestSize);
        buffer = chan.cread(resultLength);
        y1output << buffer;
    }

    //set the request length to the ending
    ((filemsg *) request)->length = size % 100;
    ((filemsg *) request)->offset = size  - (size % 100);
    chan.cwrite(request, requestSize);

    buffer = chan.cread(resultLength);
    y1output << buffer;
    y1output.close();

}

void test3(FIFORequestChannel &chan){
   	//3. request binary file
    cout << "TEST 3: copying BIMDC/mybin to BinCpyOutput" << endl;
    char * fileName = "mybin";
    char * ofileName = "BinCpyOutput";
    filemsg *fmsg = new filemsg(0, 0);
	int requestSize = sizeof(filemsg) + sizeof(char) * 5 + 1;
    
    char * request = new char[requestSize];
    *((filemsg *) request) = *fmsg;

    strcpy(request + sizeof(filemsg), fileName);
    chan.cwrite(request, requestSize);

    int * resultLength = new int(0);
    int size = *((int*)chan.cread(resultLength));
    
    //send a request for information (person, seconds, ecg number)
    ((filemsg *) request)->length = 100;
    ((filemsg *) request)->offset = 0;

    //file setup
	FILE* y1output = fopen (ofileName, "wb");

    //loop and request
    char *buffer;
    for(int i = 100; i < size; i+= 100){
        ((filemsg *) request)->offset = i - 100;

        chan.cwrite(request, requestSize);
        buffer = chan.cread(resultLength);

		fseek (y1output, ((filemsg *) request)->offset, SEEK_SET);
		fwrite (buffer, 1, 100, y1output);
    }

	if(size % 100 != 0){

		//set the request length to the ending
		((filemsg *) request)->length = size % 100;
		((filemsg *) request)->offset = size  - (size % 100);
		
		chan.cwrite(request, requestSize);

		buffer = chan.cread(resultLength);
		fseek (y1output, ((filemsg *) request)->offset, SEEK_SET);
		fwrite (buffer, 1, size % 100, y1output);
	}
 
    fclose(y1output);
}

void test4(FIFORequestChannel &chan){

	//4. Requesting a new channel
	datamsg *request = new datamsg(1, 0, 1);
	request->mtype = NEWCHANNEL_MSG;
	chan.cwrite((char *) request, sizeof(datamsg));

	int *size = new int();
	char *requestedChannel = chan.cread(size);
	cout << "TEST 4: creating new channel named: " << requestedChannel << endl;
    FIFORequestChannel customChannel (requestedChannel, FIFORequestChannel::CLIENT_SIDE);

    ofstream person1output("customRequest.csv");
    int *len = new int(100);
    char *buffer = new char(100);

    //send a request for information (person, seconds, ecg number)
    int i = 0;
    *request = datamsg(1, (double)i * 0.004, 2);
    person1output << (double)i * 0.004 << ",";

    chan.cwrite((char*)request, sizeof(datamsg));
    buffer = chan.cread(len);
    person1output << *((double*)buffer) << ",";

    *request = datamsg(1, (double)i * 0.004, 2);
    chan.cwrite((char*)request, sizeof(datamsg));
    buffer = chan.cread(len);
    person1output << *((double*)buffer) << endl;

    delete len;
    person1output.close();
}
