#include "dev-Prefix.h"
#include "spriteTask.h"
#include "sliderTask.h"
#include "lwfk/lwlog.h"
#include "lwfk/lwEffects.h"
#include "lwfk/lwFileSys.h"
#include "lwfk/lwSprite.h"
#include "lwfk/lwModel.h"
#include "lwfk/lwCamera.h"


SpriteTask gSpriteTask;

SpriteTask::SpriteTask() {
    
}


SpriteTask::~SpriteTask() {
    
}


void SpriteTask::vStart() {
    _pSprite = lw::Sprite::createFromFile("img05.png");
    _pSprite->setUV(0.f, 0.f, 256.f, 256.f);
    _pSprite->setPos(0, 0);

    _pModel = lw::Model::create("girl.lwmdl");
    _pCamera = new lw::Camera();
    lw::Camera::setCurrent(_pCamera);
    PVRTVECTOR3 eye = {10.f, 10.f, 10.f};
    PVRTVECTOR3 at = {0.f, 5.f, 0.f};
    PVRTVECTOR3 up = {0.f, 1.f, 0.f};
    _pCamera->lookat(eye, at, up);
    _pCamera->perspective(PVRT_PIf/3.f, 9.0f/16.0f, 1, 1000);
}


void SpriteTask::vStop() {
    delete _pSprite;
    delete _pModel;
    delete _pCamera;
}


void SpriteTask::vUpdate() {
}


void SpriteTask::vDraw() {
    glClearColor(1.f, 0.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
//    static float t = 0;
//    t += .1f;
//    _pSprite->setPos(sin(t)*100.f + 100.f, 0);
//    _pSprite->draw();
//    
//    _pSprite->setPos(640-256, 1136-256);
//    _pSprite->draw();
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    _pModel->draw(*_pCamera);
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







