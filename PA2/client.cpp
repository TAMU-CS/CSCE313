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

    //requesting data points for person 1
    //create output file
    ofstream person1output("x1.csv");
    int *len = new int(100);
    char *buffer = new char(100);

    double curTime = 0;
    for(int i = 0; i < 15000; i++){ //15000 different times
        //send a request for information (person, seconds, ecg number)
        datamsg *request = new datamsg(DATA_MSG, 60, 1);

        chan.cwrite((char*)request, 100);

        buffer = chan.cread();
        cout << "yoyoyoy" << endl;

        curTime += 0.04;

    }
    delete len;

	return 0;
}
