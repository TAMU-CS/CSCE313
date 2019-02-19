/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/19
 */
#include "common.h"
#include "FIFOreqchannel.h"

using namespace std;


int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);

    /*1. requesting data points for person 1
    //create output file
    ofstream person1output("x1.csv");
    int *len = new int(100);
    char *buffer = new char(100);

    for(int i = 0; i < 15000; i++){ //15000 different times
        //send a request for information (person, seconds, ecg number)
        datamsg *request = new datamsg(1, (double)i * 0.004, 1);
        person1output << (double)i * 0.004 << ",";

        chan.cwrite((char*)request, 100);
        buffer = chan.cread(len);
        person1output << *((double*)buffer) << ",";

        *request = datamsg(1, (double)i * 0.004, 2);
        chan.cwrite((char*)request, 100);
        buffer = chan.cread(len);
        person1output << *((double*)buffer) << endl;

        delete request;
    }
    delete len;
    person1output.close();
    */

    /*2. request a test File
    //get the length
    char * fileName = "1.csv";
    filemsg *fmsg = new filemsg(0, 0);
    
    char * request = new char[sizeof(filemsg) + sizeof(char) * 5 + 1];
    *((filemsg *) request) = *fmsg;

    strcpy(request + sizeof(filemsg), fileName);
    chan.cwrite(request, sizeof(filemsg) + sizeof(char) * 5 + 1);

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

        chan.cwrite(request, sizeof(filemsg) + sizeof(char) * 5 + 1);
        buffer = chan.cread(resultLength);
        y1output << buffer;
    }

    //set the request length to the ending
    ((filemsg *) request)->length = size % 100;
    ((filemsg *) request)->offset = size  - (size % 100);
    chan.cwrite(request, sizeof(filemsg) + sizeof(char) * 5 + 1);

    buffer = chan.cread(resultLength);
    y1output << buffer;
    y1output.close();
    */

   	/*3. request binary file

    char * fileName = "mybin";
    filemsg *fmsg = new filemsg(0, 0);
	int requestSize = sizeof(filemsg) + sizeof(char) * 5 + 1;
    
    char * request = new char[requestSize];
    *((filemsg *) request) = *fmsg;

    strcpy(request + sizeof(filemsg), fileName);
    chan.cwrite(request, requestSize);

    int * resultLength = new int(0);
    int size = *((int*)chan.cread(resultLength));
	cout << size << endl;
    //send a request for information (person, seconds, ecg number)
    ((filemsg *) request)->length = 100;
    ((filemsg *) request)->offset = 0;

    //file setup
	FILE* y1output = fopen ("BinCpyOutput", "wb");

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
	*/

	/*4. Requesting a new channel
	datamsg *request = new datamsg(1, 0, 1);
	request->mtype = NEWCHANNEL_MSG;
	chan.cwrite((char *) request, sizeof(datamsg));

	int *size = new int();
	char *requestedChannel = chan.cread(size);
	cout << requestedChannel << endl;
    FIFORequestChannel customChannel (requestedChannel, FIFORequestChannel::CLIENT_SIDE);

    ofstream person1output("customRequest.csv");
    int *len = new int(100);
    char *buffer = new char(100);

    for(int i = 0; i < 100; i++){ //15000 different times
        //send a request for information (person, seconds, ecg number)
        *request = datamsg(1, (double)i * 0.004, 2);
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
    person1output.close();
	*/

	/*5.Run server as child process and then send quit msg
	*/
	int cid = fork(); //create child process

	if(cid){ //parent process
		
	}else{ //child process
		char * argv[] = {"./dataserver"};
		execvp("./dataserver", argv);
		sleep(10);
		
	}

	sleep(10); //sleep for 10 seconds

	return 0;
}
