#include "dev-Prefix.h"
#include "spriteTask.h"
#include "sliderTask.h"
#include "lwfk/lwlog.h"
#include "lwfk/lwEffects.h"
#include "lwfk/lwFileSys.h"


SpriteTask gSpriteTask;

SpriteTask::SpriteTask() {
    ainfo("SpriteTask()");
}


SpriteTask::~SpriteTask() {
    //lwinfo("~SpriteTask()");
}


void SpriteTask::vStart() {
//    lwinfo("vStart()");
//    lw::EffectsRes *pFxRes = lw::EffectsRes::create("sprite.lwfx");
//    lw::EffectsRes *pFxRes2 = lw::EffectsRes::create("sprite.lwfx");
//    pFxRes->release();
//    pFxRes2->release();
//    
    _pSprite = lw::Sprite::createFromFile("img05.png");
    _pSprite->setUV(0.f, 0.f, 256.f, 256.f);
    _pSprite->setPos(0, 0);
//
//    
//    std::map<int, int> _imap;
//    std::map<int, int>::iterator it;
//    it = _imap.insert(std::make_pair(1, 11)).first;
//    lwinfo(_imap.size());
//    it = _imap.insert(std::make_pair(1, 111)).first;
//    lwinfo(_imap.size());
}


void SpriteTask::vStop() {
//    lwinfo("vStop()");
    delete _pSprite;
}


void SpriteTask::vUpdate() {
}


void SpriteTask::vDraw() {
    glClearColor(1.f, 0.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static float t = 0;
    t += .1f;
    _pSprite->setPos(sin(t)*100.f + 100.f, 0);
    _pSprite->draw();
    
    _pSprite->setPos(640-256, 1136-256);
    _pSprite->draw();
}

void SpriteTask::vTouchBegan(const lw::Touch &touch) {
//    lwinfo("vTouchBegan:" << touch.x << "," << touch.y);
//    stop();
//    gSliderTask.start();
}

void SpriteTask::vTouchMoved(const lw::Touch &touch) {
//    lwinfo("vTouchMoved:" << touch.x << "," << touch.y);
}

void SpriteTask::vTouchEnded(const lw::Touch &touch)  {
//    lwinfo("vTouchEnded:" << touch.x << "," << touch.y);
}

void SpriteTask::vTouchCanceled(const lw::Touch &touch)  {
//    lwinfo("vTouchCanceled:" << touch.x << "," << touch.y);
}







