#ifndef __LW_TITLE_TASK_H__
#define __LW_TITLE_TASK_H__

#include "lwfk/lwTask.h"

class TitleSlider;

class TitleTask: public lw::Task
{
public:
    TitleTask();
    ~TitleTask();
    
private:
    virtual void vStart();
    virtual void vStop();
    virtual void vUpdate();
    virtual void vDraw();
    
    void vTouchBegan(const lw::Touch &touch);
    void vTouchMoved(const lw::Touch &touch);
    void vTouchEnded(const lw::Touch &touch);
    void vTouchCanceled(const lw::Touch &touch);
    
    TitleSlider *_pTitleSlider;
};

extern TitleTask gTitleTask;


#endif //__LW_TITLE_TASK_H__