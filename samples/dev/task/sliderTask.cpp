#include "dev-Prefix.h"
#include "sliderTask.h"
#include "spriteTask.h"
#include "lwfk/lwlog.h"
#include "lwfk/lwEffects.h"
#include "lwfk/lwFileSys.h"


SliderTask gSliderTask;

SliderTask::SliderTask() {
//    lwinfo("SliderTask()");
}


SliderTask::~SliderTask() {
//    lwinfo("~SliderTask()");
}


void SliderTask::vStart() {
//    lwinfo("vStart()");
//    lw::EffectsRes *pFxRes = lw::EffectsRes::create("sprite.lwfx");
//    lw::EffectsRes *pFxRes2 = lw::EffectsRes::create("sprite.lwfx");
//    pFxRes->release();
//    pFxRes2->release();
//    
//    _pSprite = lw::Sprite::createFromFile("img05.png");
//    _pSprite->setUV(0.f, 0.f, 256.f, 256.f);
//    _pSprite->setPos(0, 0);
//    
//    
//    std::map<int, int> _imap;
//    std::map<int, int>::iterator it;
//    it = _imap.insert(std::make_pair(1, 11)).first;
//    lwinfo(_imap.size());
//    it = _imap.insert(std::make_pair(1, 111)).first;
//    lwinfo(_imap.size());
}


void SliderTask::vStop() {
//    lwinfo("vStop()");
//    delete _pSprite;
}


void SliderTask::vUpdate() {
}


void SliderTask::vDraw() {
    glClearColor(1.f, 1.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
//    _pSprite->setPos(0, 0);
//    _pSprite->draw();
//    
//    _pSprite->setPos(640-256, 1136-256);
//    _pSprite->draw();
}

void SliderTask::vTouchBegan(const lw::Touch &touch) {
    lwinfo("vTouchBegan:" << touch.x << "," << touch.y);
    stop();
    gSpriteTask.start();
}

void SliderTask::vTouchMoved(const lw::Touch &touch) {
    lwinfo("vTouchMoved:" << touch.x << "," << touch.y);
}

void SliderTask::vTouchEnded(const lw::Touch &touch)  {
    lwinfo("vTouchEnded:" << touch.x << "," << touch.y);
}

void SliderTask::vTouchCanceled(const lw::Touch &touch)  {
    lwinfo("vTouchCanceled:" << touch.x << "," << touch.y);
}







