#include "dev-Prefix.h"
#include "sliderTask.h"
#include "spriteTask.h"
#include "lwfk/lwlog.h"
#include "lwfk/lwEffects.h"
#include "lwfk/lwSprite.h"
#include "lwfk/lwTexture.h"
#include "lwfk/lwApp.h"
#include "lwfk/lwSound.h"
#include "lwfk/lwText.h"

#include "PVR/tools/PVRTResourceFile.h"
#include <dirent.h>

struct Tile {
    Tile(const char *file, float x, float y, float u, float v, float w, float h, float rotate, float scale, int index);
    ~Tile();
    
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

Tile::Tile(const char *file, float xx, float yy, float u, float v, float w, float h, float rotate, float scale, int idx) {
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

Tile::~Tile() {
    delete pSprite;
}

void Tile::setX(float xx) {
    x = xx;
    pSprite->setPos(x + offsetX, y);
}

void Tile::setY(float yy) {
    y = yy;
    pSprite->setPos(x + offsetX, y);
}

void Tile::update() {
    if (t == 1.f)
        return;
    t += .1f;
    t = fminf(t, 1.f);
    setY(fromY+(toY-fromY)*(sinf(t*M_PI_2)));
}

void Tile::draw() {
    pSprite->draw();
}

//===================================
class SliderGame {
public:
    SliderGame();
    ~SliderGame();
    void load(const char *file);
    void clear();
    void update();
    void draw();
    
    void touchBegan(const lw::Touch &touch);
    void touchMoved(const lw::Touch &touch);
    void touchEnded(const lw::Touch &touch);
    
private:
    std::vector<Tile*> _tiles;
    const lw::Touch *_pTouch;
    Tile *_pLastTouched;
    float _blockHeight;
    float _boundX;
    float _boundY;
    float _boundW;
    float _boundH;
    int _blockNum;
    lw::Sound *_pSndTick;
    lw::Text *_pLabel;
};

SliderGame::SliderGame():_pTouch(NULL) {
    PVRTVec2 ss = lw::screenSize();
    _boundX = 0;
    _boundY = 0;
    _boundW = ss.x;
    _boundH = ss.y;
    _blockNum = 10;

    _pSndTick = new lw::Sound("button-50.wav");
    
    _pLabel = lw::Text::create("a.fnt");
    _pLabel->setPos(0, 0);
    _pLabel->setText("henyouqian");
    _pLabel->setColor(lw::Color(1.f, 0.f, 0.f, 1.f));
}

SliderGame::~SliderGame() {
    clear();
    delete _pSndTick;
    delete _pLabel;
}

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

void SliderGame::load(const char *file) {
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
        float blockUVH = uvh/_blockNum;
        _blockHeight = _boundH/_blockNum;
        float scale = _boundW / uvw;
        for (int i = 0; i < _blockNum; ++i) {
            Tile *pTile = new Tile(file, 0, _boundY+i*_blockHeight, uvu, uvv+_idxs[i]*blockUVH, uvw, blockUVH, 0, scale, _idxs[i]);
            _tiles.push_back(pTile);
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
            Tile *pTile = new Tile(file, _boundX+offsetX, _boundY+i*_blockHeight, uvu+_idxs[i]*blockUVW, uvv, blockUVW, uvh, rotate, scale, _idxs[i]);
            _tiles.push_back(pTile);
        }
    }
    
    pTexture->release();
}

void SliderGame::clear() {
    std::vector<Tile*>::iterator it = _tiles.begin();
    std::vector<Tile*>::iterator itend = _tiles.end();
    for (; it != itend; ++it) {
        delete (*it);
    }
    _tiles.clear();
}

void SliderGame::update() {
    std::vector<Tile*>::iterator it = _tiles.begin();
    std::vector<Tile*>::iterator itend = _tiles.end();
    for (;it != itend; ++it) {
        (*it)->update();
    }
}

void SliderGame::draw() {
    Tile *pTouched = NULL;
    std::vector<Tile*>::iterator it = _tiles.begin();
    std::vector<Tile*>::iterator itend = _tiles.end();
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
    
    _pLabel->draw();
}

void SliderGame::touchBegan(const lw::Touch &touch) {
    if (_pTouch)
        return;
    
    int tilesNum = (int)_tiles.size();
    for (int i = 0; i < tilesNum; ++i) {
        Tile *pTile = _tiles[i];
        float ymin = pTile->y;
        float ymax = ymin + _blockHeight;
        if (touch.y >= ymin && touch.y < ymax) {
            _pTouch = &touch;
            pTile->pTouch = _pTouch;
            pTile->touchOffsetY = touch.y - ymin;
            pTile->setX(3);
            _pLastTouched = pTile;
            break;
        }
    }
}

void SliderGame::touchMoved(const lw::Touch &touch) {
    if (_pTouch != &touch)
        return;
    
    int len = (int)_tiles.size();
    for (int i = 0;i < len; ++i) {
        Tile *pTile = _tiles[i];
        if (pTile->pTouch == _pTouch) {
            pTile->setY(touch.y - pTile->touchOffsetY);
            
            //check tiles order change
            float y = pTile->y - _boundY;
            y = y + _blockHeight * .5f;
            int posIdx = floorf(y/_blockHeight);
            posIdx = std::max(0, std::min(_blockNum-1, posIdx));
            if (posIdx != i) {
                _pSndTick->play();
                
                std::vector<Tile*>::iterator it = _tiles.begin();
                it += i;
                _tiles.erase(it);
                it = _tiles.begin();
                it += posIdx;
                _tiles.insert(it, pTile);
                
                for (int iTile = 0; iTile < len; ++iTile) {
                    Tile *pTile = _tiles[iTile];
                    if (pTile->pTouch == NULL) {
                        float toY = _boundY + _blockHeight * iTile;
                        if (pTile->y != toY && pTile->toY != toY) {
                            pTile->fromY = pTile->y;
                            pTile->toY = toY;
                            pTile->t = 0.f;
                        }
                    }
                }
            }
            break;
        }
    }
}

void SliderGame::touchEnded(const lw::Touch &touch) {
    if (_pTouch == NULL || _pTouch != &touch)
        return;
    
    int len = (int)_tiles.size();
    for (int i = 0;i < len; ++i) {
        Tile *pTile = _tiles[i];
        if (pTile->pTouch == _pTouch) {
            _pTouch = pTile->pTouch = NULL;
            pTile->setX(0);
            
            pTile->t = 0;
            pTile->fromY = pTile->y;
            pTile->toY = _boundY + _blockHeight * i;
            _pLastTouched = pTile;
            break;
        }
    }
}

//============================================


SliderTask gSliderTask;

SliderTask::SliderTask() {
//    lwinfo("SliderTask()");
}


SliderTask::~SliderTask() {
//    lwinfo("~SliderTask()");
}

namespace {
    const char *IMAGE_FOLDER = "img/";
}

void SliderTask::vStart() {
    _pSliderGame = new SliderGame();
    
    //collect all image names
    std::string path = CPVRTResourceFile::GetReadPath().c_str();
    path.append(IMAGE_FOLDER);
    DIR *pDir = opendir(path.c_str());
    struct dirent *pDirent;
    while (1) {
        pDirent = readdir(pDir);
        if (pDirent == NULL)
            break;
        const char *p = strrchr(pDirent->d_name, '.');
        if (p && strcmp(p, ".jpg") == 0) {
            _fileNames.push_back(pDirent->d_name);
        }
    }
    closedir(pDir);
    
    //random pick a image
    srand(time(NULL));
    path = IMAGE_FOLDER;
//    int imgIdx = rand() % _fileNames.size();    
//    path.append(_fileNames[imgIdx]);
    path.append("tumblr_mqi7j5ltYz1rxobalo2_r1_1280_x.jpg");
    _pSliderGame->load(path.c_str());
}


void SliderTask::vStop() {
    delete _pSliderGame;
}


void SliderTask::vUpdate() {
    _pSliderGame->update();
}


void SliderTask::vDraw() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _pSliderGame->draw();
}

void SliderTask::vTouchBegan(const lw::Touch &touch) {
    if (touch.y > lw::screenSize().y - 40 && touch.x < 40) {
        _pSliderGame->clear();
        //random pick a image
        srand(time(NULL));
        int imgIdx = rand() % _fileNames.size();
        std::string path = IMAGE_FOLDER;
        path.append(_fileNames[imgIdx]);
        _pSliderGame->load(path.c_str());
        return;
    }
    _pSliderGame->touchBegan(touch);
}

void SliderTask::vTouchMoved(const lw::Touch &touch) {
    _pSliderGame->touchMoved(touch);
}

void SliderTask::vTouchEnded(const lw::Touch &touch)  {
    _pSliderGame->touchEnded(touch);

}

void SliderTask::vTouchCanceled(const lw::Touch &touch)  {
    _pSliderGame->touchEnded(touch);
}





