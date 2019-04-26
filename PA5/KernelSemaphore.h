#ifndef _KernelSemaphore_H_
#define _KernelSemaphore_H_

#include "common.h"

//abstract class definition of request channel
class KernelSemaphore
{
private:
    int val;
public:
    //constructor
    KernelSemaphore(int _val){
        val = _val;
    };

    //destructor
    ~KernelSemaphore(){

    };

    // acquire lock
    void P(){

    }; 

    // release lock
    void V(){

    }; 
};

#endif
