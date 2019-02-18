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
    char * fileName = "1.csv\0";
    filemsg *fmsg = new filemsg(50, 28);
    
    char * request = new char[sizeof(filemsg) + sizeof(char) * 5 + 1];
    *((filemsg *) request) = *fmsg;

    strcpy(request + sizeof(filemsg), fileName);
    chan.cwrite(request, sizeof(filemsg) + sizeof(char) * 5 + 1);

    int * resultLength = new int(0);
    cout << *((int*)chan.cread(resultLength)) << endl;

	return 0;
}
