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

    //2. request a test File
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

	return 0;
}
