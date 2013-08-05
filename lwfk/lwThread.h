#ifndef __LW_THREAD_H__
#define __LW_THREAD_H__

namespace lw {
    class Thread{
    public:
        Thread(void *userData = NULL);
        virtual ~Thread();
        virtual void vMain() = 0;
        
    protected:
        pthread_t _threadID;
        void * _userData;
    };
}

#endif //__LW_THREAD_H__