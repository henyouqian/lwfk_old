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
    
    _pSprite1 = lw::Sprite::createFromFile("img05.png", "add");
    _pSprite1->setUV(0.f, 0.f, 256.f, 256.f);

    _pModel = lw::Model::create("girl.lwmdl");
    _pCamera = new lw::Camera();
    lw::Camera::setCurrent(_pCamera);
    _pCamera->perspective(PVRT_PIf/3.f, 9.0f/16.0f, 1, 1000);
}


void SpriteTask::vStop() {
    delete _pSprite;
    delete _pSprite1;
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
    
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_DEPTH_TEST);
//    glDepthMask(GL_TRUE);
//    glDepthFunc(GL_LESS);
//    glDisable(GL_BLEND);
    
    _pModel->draw(*_pCamera);
    
    
    _pSprite->setAnchor(0, 0);
    _pSprite->setAnchor(128, 128);
    _pSprite->setPos(sinf(t)*100.f+100.f, 128);
    _pSprite->setRotate(0);
    _pSprite->setScale(1.f, 1.f);
    _pSprite->setColor(lw::Color(1.f, 1.f, 1.f, .3f));
//    lw::rsBlend(true);
//    lw::rsBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    lw::rsFlush();
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    _pSprite->draw();
//    _pSprite->flush();
    
    _pSprite1->setAnchor(128, 128);
    _pSprite1->setPos(640-256, 1136-256);
    _pSprite1->setRotate(t);
    float s = sinf(t*.3f)*.5+1.f;
    _pSprite1->setScale(s, s);
    _pSprite1->setColor(lw::Color(1.f, 1.f, 0.f, s-.5f));
//    lw::rsBlend(true);
//    lw::rsBlendFunc(GL_SRC_ALPHA, GL_ONE);
//    lw::rsFlush();
    _pSprite1->draw();
}

void SpriteTask::vTouchBegan(const lw::Touch &touch) {
//    stop();
//    gSliderTask.start();
    lwinfo("vTouchBegan:%f, %f", touch.x, touch.y);
}

void SpriteTask::vTouchMoved(const lw::Touch &touch) {
    lwinfo("vTouchMoved:%f, %f, %f, %f", touch.x, touch.y, touch.prevX, touch.prevY);
}

void SpriteTask::vTouchEnded(const lw::Touch &touch)  {
    lwinfo("vTouchEnded:%f, %f", touch.x, touch.y);
}

void SpriteTask::vTouchCanceled(const lw::Touch &touch)  {
   lwinfo("vTouchCanceled:%f, %f", touch.x, touch.y);
}







