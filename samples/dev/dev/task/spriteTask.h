#ifndef __LW_SPRITE_TASK_H__
#define __LW_SPRITE_TASK_H__

#include "lwfk/lwTask.h"
#include "lwfk/lwSprite.h"

class SpriteTask: public lw::Task
{
public:
    SpriteTask();
    ~SpriteTask();
    
private:
    virtual void vStart();
    virtual void vStop();
    virtual void vUpdate();
    virtual void vDraw();
    
    void vTouchBegan(const lw::Touch &touch);
    void vTouchMoved(const lw::Touch &touch);
    void vTouchEnded(const lw::Touch &touch);
    void vTouchCanceled(const lw::Touch &touch);
    
    lw::Sprite *_pSprite;
};

extern SpriteTask gSpriteTask;


#endif //__LW_TASK_H__