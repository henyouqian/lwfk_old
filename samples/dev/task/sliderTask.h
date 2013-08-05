#ifndef __SLIDER_TASK_H__
#define __SLIDER_TASK_H__

#include "lwfk/lwTask.h"

class SliderGame;

class SliderTask: public lw::Task
{
public:
    SliderTask();
    ~SliderTask();
    
private:
    virtual void vStart();
    virtual void vStop();
    virtual void vUpdate();
    virtual void vDraw();
    
    void vTouchBegan(const lw::Touch &touch);
    void vTouchMoved(const lw::Touch &touch);
    void vTouchEnded(const lw::Touch &touch);
    void vTouchCanceled(const lw::Touch &touch);
    
    SliderGame *_pSliderGame;
    std::vector<std::string> _fileNames;
};

extern SliderTask gSliderTask;


#endif //__SLIDER_TASK_H__