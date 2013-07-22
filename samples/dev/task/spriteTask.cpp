#include "dev-Prefix.h"
#include "spriteTask.h"
#include "sliderTask.h"
#include "lwfk/lwlog.h"
#include "lwfk/lwEffects.h"
#include "lwfk/lwFileSys.h"
#include "lwfk/lwSprite.h"
#include "lwfk/lwModel.h"
#include "lwfk/lwCamera.h"
#include "lwfk/lwRenderState.h"

#include <math.h>


SpriteTask gSpriteTask;

SpriteTask::SpriteTask() {
    
}


SpriteTask::~SpriteTask() {
    
}


void SpriteTask::vStart() {
    _pSprite = lw::Sprite::createFromFile("img05.png", "normal");
    _pSprite->setUV(0.f, 0.f, 256.f, 256.f);
    _pSprite->setPos(0, 0);

    _pModel = lw::Model::create("girl.lwmdl");
    _pCamera = new lw::Camera();
    lw::Camera::setCurrent(_pCamera);
    _pCamera->perspective(PVRT_PIf/3.f, 9.0f/16.0f, 1, 1000);
}


void SpriteTask::vStop() {
    delete _pSprite;
    delete _pModel;
    delete _pCamera;
}


void SpriteTask::vUpdate() {
    static float f = 0.0f;
    f += 0.03f;
    PVRTVec3 eye(sinf(f)*10.f, 10.f, cosf(f)*10.f);
    PVRTVec3 at(0.f, 5.f, 0.f);
    PVRTVec3 up(0.f, 1.f, 0.f);
    _pCamera->lookat(eye, at, up);
    
    
}


void SpriteTask::vDraw() {
    glClearColor(1.f, 0.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static float t = 0;
    t += .1f;
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    
    _pModel->draw(*_pCamera);
    
    
    _pSprite->setAnchor(0, 0);
    _pSprite->setAnchor(128, 128);
    _pSprite->setPos(sinf(t)*100.f+100.f, 128);
    _pSprite->setRotate(0);
    _pSprite->setScale(1.f, 1.f);
    _pSprite->setColor(lw::Color(1.f, 1.f, 1.f, .1f));
    lw::rsEnableBlend();
    lw::rsFlush();
    _pSprite->draw();
    _pSprite->flush();
    
    _pSprite->setAnchor(128, 128);
    _pSprite->setPos(640-256, 1136-256);
    _pSprite->setRotate(t);
    float s = sinf(t*.3f)*.5+1.f;
    _pSprite->setScale(s, s);
    _pSprite->setColor(lw::Color(1.f, 1.f, 0.f, 1.f));
    lw::rsEnableBlend();
    lw::rsBlendFunc(GL_ONE, GL_ONE);
    lw::rsFlush();
    _pSprite->draw();
}

void SpriteTask::vTouchBegan(const lw::Touch &touch) {
//    lwinfo("vTouchBegan:" << touch.x << "," << touch.y);
//    stop();
//    gSliderTask.start();
    lwinfo("vTouchBegan:%f, %f", touch.x, touch.y);
}

void SpriteTask::vTouchMoved(const lw::Touch &touch) {
//    lwinfo("vTouchMoved:" << touch.x << "," << touch.y);
    lwinfo("vTouchMoved:%f, %f", touch.x, touch.y);
}

void SpriteTask::vTouchEnded(const lw::Touch &touch)  {
//    lwinfo("vTouchEnded:" << touch.x << "," << touch.y);
}

void SpriteTask::vTouchCanceled(const lw::Touch &touch)  {
//    lwinfo("vTouchCanceled:" << touch.x << "," << touch.y);
}







