#include "dev-Prefix.h"
#include "TitleTask.h"
#include "sliderTask.h"
#include "lwfk/lwlog.h"
#include "lwfk/lwTexture.h"
#include "lwfk/lwSprite.h"
#include "lwfk/lwRenderState.h"
#include "lwfk/lwSound.h"
#include "lwfk/lwText.h"
#include "lwfk/lwApp.h"

#include <math.h>


TitleTask gTitleTask;

struct TitleTile {
    TitleTile(const char *file, float x, float y, float u, float v, float w, float h, float rotate, float scale, int index);
    ~TitleTile();
    
    void update();
    void draw();
    
    void setX(float x);
    void setY(float y);
    
    int index;
    lw::Sprite * pSprite;
    float offsetX;
    float x, y;
    float touchOffsetY;
    const lw::Touch *pTouch;
    
    float t;
    float fromY;
    float toY;
};

TitleTile::TitleTile(const char *file, float xx, float yy, float u, float v, float w, float h, float rotate, float scale, int idx) {
    pSprite = lw::Sprite::createFromFile(file, "normal");
    assert(pSprite);
    pSprite->setUV(u, v, w, h);
    pSprite->setPos(xx, yy);
    pSprite->setRotate(rotate);
    pSprite->setScale(scale, scale);
    offsetX = xx;
    x = 0;
    y = yy;
    pTouch = NULL;
    touchOffsetY = 0;
    t = 1.f;
    index = idx;
}

TitleTile::~TitleTile() {
    delete pSprite;
}

void TitleTile::setX(float xx) {
    x = xx;
    pSprite->setPos(x + offsetX, y);
}

void TitleTile::setY(float yy) {
    y = yy;
    pSprite->setPos(x + offsetX, y);
}

void TitleTile::update() {
    if (t == 1.f)
        return;
    t += .1f;
    t = fminf(t, 1.f);
    setY(fromY+(toY-fromY)*(sinf(t*M_PI_2)));
}

void TitleTile::draw() {
    pSprite->draw();
}

//===================================
class TitleSlider {
public:
    TitleSlider();
    ~TitleSlider();
    void load(const char *file);
    void clear();
    void update();
    void draw();
    
    void touchBegan(const lw::Touch &touch);
    void touchMoved(const lw::Touch &touch);
    bool touchEnded(const lw::Touch &touch);
    
private:
    std::vector<TitleTile*> _TitleTiles;
    const lw::Touch *_pTouch;
    TitleTile *_pLastTouched;
    float _blockHeight;
    float _boundX;
    float _boundY;
    float _boundW;
    float _boundH;
    int _blockNum;
    lw::Sound *_pSndTick;
    bool _moved;
};

TitleSlider::TitleSlider():_pTouch(NULL) {
    PVRTVec2 ss = lw::screenSize();
    _boundX = 0;
    _boundY = 0;
    _boundW = ss.x;
    _boundH = ss.y;
    _blockNum = 10;
    
    _pSndTick = new lw::Sound("button-50.wav");
    srand(time(NULL));
}

TitleSlider::~TitleSlider() {
    clear();
    delete _pSndTick;
}

namespace {
    void shuffle(std::vector<int> &vec, int num)
    {
        vec.clear();
        std::vector<int> v;
        for (int i = 0; i < num; ++i) {
            v.push_back(i);
        }
        for (int i = 0; i < num; ++i) {
            int r = rand() % v.size();
            vec.push_back(v[r]);
            v.erase(v.begin()+r);
        }
    }
}


void TitleSlider::load(const char *file) {
    _pTouch = NULL;
    _pLastTouched = NULL;
    
    lwinfo("xxxxx:%s", file);
    
    lw::TextureRes *pTexture = lw::TextureRes::create(file);
    float textureWidth = (float)pTexture->getWidth();
    float textureHeight = (float)pTexture->getHeight();
    
    float uvu, uvv;
    float uvw, uvh;
    
    std::vector<int> _idxs;
    shuffle(_idxs, _blockNum);
    
    if (textureWidth <= textureHeight) {
        if (textureWidth/textureHeight <= _boundW/_boundH) {
            uvw = textureWidth;
            uvh = textureWidth * _boundH / _boundW;
            uvu = 0.f;
            uvv = (textureHeight - uvh) * .5f;
        } else {
            uvw = textureHeight * _boundW / _boundH;
            uvh = textureHeight;
            uvu = (textureWidth - uvw) * .5f;
            uvv = 0.f;
        }
//        float blockUVH = uvh/_blockNum;
        float blockUVH = 114.f;
//        _blockHeight = _boundH/_blockNum;
        _blockHeight = 114.f;
        float scale = _boundW / uvw;
        for (int i = 0; i < _blockNum; ++i) {
            TitleTile *pTitleTile = new TitleTile(file, 0, _boundY+i*_blockHeight, uvu, uvv+_idxs[i]*blockUVH, uvw, blockUVH, 0, scale, _idxs[i]);
            _TitleTiles.push_back(pTitleTile);
        }
    } else {
        float rotate = -M_PI_2;
        float offsetX = _boundW;
        if (textureWidth/textureHeight <= _boundH/_boundW) {
            uvw = textureWidth;
            uvh = textureWidth * _boundW / _boundH;
            uvu = 0.f;
            uvv = (textureHeight - uvh) * .5f;
        } else {
            uvw = textureHeight * _boundH / _boundW;
            uvh = textureHeight;
            uvu = (textureWidth - uvw) * .5f;
            uvv = 0.f;
        }
        float blockUVW = uvw/_blockNum;
        _blockHeight = _boundH/_blockNum;
        float scale = _boundH / uvw;
        for (int i = 0; i < _blockNum; ++i) {
            TitleTile *pTitleTile = new TitleTile(file, _boundX+offsetX, _boundY+i*_blockHeight, uvu+_idxs[i]*blockUVW, uvv, blockUVW, uvh, rotate, scale, _idxs[i]);
            _TitleTiles.push_back(pTitleTile);
        }
    }
    
    pTexture->release();
}

void TitleSlider::clear() {
    std::vector<TitleTile*>::iterator it = _TitleTiles.begin();
    std::vector<TitleTile*>::iterator itend = _TitleTiles.end();
    for (; it != itend; ++it) {
        delete (*it);
    }
    _TitleTiles.clear();
}

void TitleSlider::update() {
    std::vector<TitleTile*>::iterator it = _TitleTiles.begin();
    std::vector<TitleTile*>::iterator itend = _TitleTiles.end();
    for (;it != itend; ++it) {
        (*it)->update();
    }
}

void TitleSlider::draw() {
    TitleTile *pTouched = NULL;
    std::vector<TitleTile*>::iterator it = _TitleTiles.begin();
    std::vector<TitleTile*>::iterator itend = _TitleTiles.end();
    for (;it != itend; ++it) {
        if ( _pLastTouched && _pLastTouched == *it) {
            pTouched = *it;
        } else {
            (*it)->draw();
        }
    }
    if (pTouched) {
        pTouched->draw();
    }
}

void TitleSlider::touchBegan(const lw::Touch &touch) {
    if (_pTouch)
        return;
    
    int TitleTilesNum = (int)_TitleTiles.size();
    for (int i = 0; i < TitleTilesNum; ++i) {
        TitleTile *pTitleTile = _TitleTiles[i];
        float ymin = pTitleTile->y;
        float ymax = ymin + _blockHeight;
        if (touch.y >= ymin && touch.y < ymax) {
            _pTouch = &touch;
            pTitleTile->pTouch = _pTouch;
            pTitleTile->touchOffsetY = touch.y - ymin;
            pTitleTile->setX(3);
            _pLastTouched = pTitleTile;
            break;
        }
    }
    _moved = false;
}

void TitleSlider::touchMoved(const lw::Touch &touch) {
    if (_pTouch != &touch)
        return;
    
    int len = (int)_TitleTiles.size();
    for (int i = 0;i < len; ++i) {
        TitleTile *pTitleTile = _TitleTiles[i];
        if (pTitleTile->pTouch == _pTouch) {
            pTitleTile->setY(touch.y - pTitleTile->touchOffsetY);
            
            //check TitleTiles order change
            float y = pTitleTile->y - _boundY;
            y = y + _blockHeight * .5f;
            int posIdx = floorf(y/_blockHeight);
            posIdx = std::max(0, std::min(_blockNum-1, posIdx));
            if (posIdx != i) {
                _pSndTick->play();
                _moved = true;
                
                std::vector<TitleTile*>::iterator it = _TitleTiles.begin();
                it += i;
                _TitleTiles.erase(it);
                it = _TitleTiles.begin();
                it += posIdx;
                _TitleTiles.insert(it, pTitleTile);
                
                for (int iTitleTile = 0; iTitleTile < len; ++iTitleTile) {
                    TitleTile *pTitleTile = _TitleTiles[iTitleTile];
                    if (pTitleTile->pTouch == NULL) {
                        float toY = _boundY + _blockHeight * iTitleTile;
                        if (pTitleTile->y != toY && pTitleTile->toY != toY) {
                            pTitleTile->fromY = pTitleTile->y;
                            pTitleTile->toY = toY;
                            pTitleTile->t = 0.f;
                        }
                    }
                }
            }
            break;
        }
    }
}

bool TitleSlider::touchEnded(const lw::Touch &touch) {
    if (_pTouch == NULL || _pTouch != &touch)
        return false;
    
    int len = (int)_TitleTiles.size();
    for (int i = 0;i < len; ++i) {
        TitleTile *pTitleTile = _TitleTiles[i];
        if (pTitleTile->pTouch == _pTouch) {
            _pTouch = pTitleTile->pTouch = NULL;
            pTitleTile->setX(0);
            
            pTitleTile->t = 0;
            pTitleTile->fromY = pTitleTile->y;
            pTitleTile->toY = _boundY + _blockHeight * i;
            _pLastTouched = pTitleTile;
            
            if (_moved) {
                lwinfo("moved");
            } else {
                return true;
            }
            break;
        }
    }
    return false;
}


TitleTask::TitleTask() {
    lwinfo("TitleTask::TitleTask()");
}


TitleTask::~TitleTask() {
    
}

void TitleTask::vStart() {
    _pTitleSlider = new TitleSlider();
    _pTitleSlider->load("title.png");
}

void TitleTask::vStop() {
    delete _pTitleSlider;
}


void TitleTask::vUpdate() {
    _pTitleSlider->update();
}


void TitleTask::vDraw() {
    glClearColor(0.31f, 0.69f, 0.61f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _pTitleSlider->draw();
}

void TitleTask::vTouchBegan(const lw::Touch &touch) {
    _pTitleSlider->touchBegan(touch);
}

void TitleTask::vTouchMoved(const lw::Touch &touch) {
    _pTitleSlider->touchMoved(touch);
}

void TitleTask::vTouchEnded(const lw::Touch &touch)  {
    if (_pTitleSlider->touchEnded(touch)) {
        stop();
        gSliderTask.start();
    }
}

void TitleTask::vTouchCanceled(const lw::Touch &touch)  {
   _pTitleSlider->touchEnded(touch);
}







