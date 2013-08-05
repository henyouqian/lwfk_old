#include "lwPrefix.h"
#include "lwThread.h"
#include "lwLog.h"
#include <pthread.h>

namespace lw {
    
    void* threadMain(void *pData) {
        Thread *pThread = (Thread*)pData;
        pThread->vMain();
        return NULL;
    }
    
    Thread::Thread(void *userData) {
        pthread_attr_t  attr;
        int             returnVal;
        
        _userData = userData;
        
        returnVal = pthread_attr_init(&attr);
        assert(!returnVal);
        returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        assert(!returnVal);
        
        int threadError = pthread_create(&_threadID, &attr, &threadMain, this);
        
        returnVal = pthread_attr_destroy(&attr);
        assert(!returnVal);
        if (threadError != 0) {
            lwerror("pthread_create error: %d", threadError);
        }
    }
    
    Thread::~Thread() {
        pthread_cancel(_threadID);
    }
    
}
