#ifndef _KernelSemaphore_H_
#define _KernelSemaphore_H_

#include "common.h"

//abstract class definition of request channel
class KernelSemaphore
{
private:
    sem_t *semid;
    string id;
public:
    //constructor
    KernelSemaphore(string _id , int _val){
        id = "/" + _id;

        semid = sem_open(id.c_str(), O_CREAT, 0666, _val);    
        if(semid == SEM_FAILED){
            perror("KernelSemaphore");
        }    
    };

    //destructor
    ~KernelSemaphore(){
        sem_close(semid);
        sem_unlink(id.c_str());
    };

    // acquire lock
    void P(){
        sem_wait(semid);
    }; 

    // release lock
    void V(){
        sem_post(semid);
    }; 
};

#endif
