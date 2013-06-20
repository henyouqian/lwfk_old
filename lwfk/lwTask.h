#ifndef __LW_TASK_H__
#define __LW_TASK_H__

#include "lwTouch.h"

namespace lw {
    
    class Task {
    public:
        Task();
        virtual ~Task();
        
        void setPriority(float priority);
        float getPriority();
        
        void start();
        void stop();
        
    private:
        virtual void vStart(){};
        virtual void vStop(){};
        virtual void vUpdate(){};
        virtual void vDraw(){};
        virtual void vTouchBegan(const lw::Touch &touch){};
        virtual void vTouchMoved(const lw::Touch &touch){};
        virtual void vTouchEnded(const lw::Touch &touch){};
        virtual void vTouchCanceled(const lw::Touch &touch){};
        
    //don't use below
    public:
        static void init();
        static void quit();
        static void update();
        static void draw();
        static void touchBegan(const Touch &touch);
        static void touchMoved(const Touch &touch);
        static void touchEnded(const Touch &touch);
        static void touchCanceled(const Touch &touch);
        
        enum State
        {
            IDLE,
            RUNNING,
            PENDING,
        };

    private:
        float _priority;
        State _state;
    };
    
} //namespace lw


#endif //__LW_TASK_H__