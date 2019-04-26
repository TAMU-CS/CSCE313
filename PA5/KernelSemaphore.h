#ifndef _KernelSemaphore_H_
#define _KernelSemaphore_H_

#include "common.h"

//abstract class definition of request channel
class KernelSemaphore
{
private:
    int val;
    int id;
public:
    //constructor
    KernelSemaphore(int _val, key_t k){
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
