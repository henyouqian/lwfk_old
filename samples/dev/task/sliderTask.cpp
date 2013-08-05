#include "dev-Prefix.h"
#include "sliderTask.h"
#include "spriteTask.h"
#include "lwfk/lwlog.h"
#include "lwfk/lwEffects.h"
#include "lwfk/lwFileSys.h"
#include "lwfk/lwSprite.h"
#include "lwfk/lwTexture.h"
#include "lwfk/lwApp.h"

#include "PVR/tools/PVRTResourceFile.h"
#include <dirent.h>

struct Tile {
    Tile(const char *file, float x, float y, float u, float v, float w, float h, float rotate, float scale);
    ~Tile();
    
    void update();
    void draw();
    
    void setX(float x);
    void setY(float y);
    
    lw::Sprite * pSprite;
    float offsetX;
    float x, y;
    float touchOffsetY;
    const lw::Touch *pTouch;
    
    float t;
    float fromY;
    float toY;
};

Tile::Tile(const char *file, float xx, float yy, float u, float v, float w, float h, float rotate, float scale) {
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
};

SliderGame::SliderGame():_pTouch(NULL) {
    
}

SliderGame::~SliderGame() {
    clear();
}

void SliderGame::load(const char *file) {
    _pTouch = NULL;
    _pLastTouched = NULL;
    
    lwinfo("xxxxx:%s", file);
    float boundX = 0;
    float boundY = 0;
    float boundW = 640;
    float boundH = 960;
    float blockNum = 10;
    
    lw::TextureRes *pTexture = lw::TextureRes::create(file);
    float textureWidth = (float)pTexture->getWidth();
    float textureHeight = (float)pTexture->getHeight();
    
    float uvu, uvv;
    float uvw, uvh;
    
    if (textureWidth <= textureHeight) {
        if (textureWidth/textureHeight <= boundW/boundH) {
            uvw = textureWidth;
            uvh = textureWidth * boundH / boundW;
            uvu = 0.f;
            uvv = (textureHeight - uvh) * .5f;
        } else {
            uvw = textureHeight * boundW / boundH;
            uvh = textureHeight;
            uvu = (textureWidth - uvw) * .5f;
            uvv = 0.f;
        }
        float blockUVH = uvh/blockNum;
        _blockHeight = boundH/blockNum;
        float y = uvv;
        float scale = boundW / uvw;
        for (int i = 0; i < blockNum; ++i) {
            Tile *pTile = new Tile(file, 0, boundY+i*_blockHeight, uvu, y, uvw, blockUVH, 0, scale);
            _tiles.push_back(pTile);
            y += blockUVH;
        }
    } else {
        float rotate = -M_PI_2;
        float offsetX = boundW;
        if (textureWidth/textureHeight <= boundH/boundW) {
            uvw = textureWidth;
            uvh = textureWidth * boundW / boundH;
            uvu = 0.f;
            uvv = (textureHeight - uvh) * .5f;
        } else {
            uvw = textureHeight * boundH / boundW;
            uvh = textureHeight;
            uvu = (textureWidth - uvw) * .5f;
            uvv = 0.f;
        }
        float blockUVW = uvw/blockNum;
        _blockHeight = boundH/blockNum;
        float x = uvu;
        float scale = boundH / uvw;
        for (int i = 0; i < blockNum; ++i) {
            Tile *pTile = new Tile(file, boundX+offsetX, boundY+i*_blockHeight, x, uvv, blockUVW, uvh, rotate, scale);
            _tiles.push_back(pTile);
            x += blockUVW;
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
    
    std::vector<Tile*>::iterator it = _tiles.begin();
    std::vector<Tile*>::iterator itend = _tiles.end();
    for (;it != itend; ++it) {
        if ((*it)->pTouch == _pTouch) {
            (*it)->setY(touch.y - (*it)->touchOffsetY);
            break;
        }
    }
}

void SliderGame::touchEnded(const lw::Touch &touch) {
    if (_pTouch == NULL || _pTouch != &touch)
        return;
    
    std::vector<Tile*>::iterator it = _tiles.begin();
    std::vector<Tile*>::iterator itend = _tiles.end();
    for (;it != itend; ++it) {
        Tile *pTile = *it;
        if (pTile->pTouch == _pTouch) {
            _pTouch = pTile->pTouch = NULL;
            pTile->setX(0);
            
            pTile->t = 0;
            pTile->fromY = pTile->y;
            pTile->toY = pTile->y - 100.f;
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
    glClearColor(1.f, 1.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _pSliderGame->draw();
}

void SliderTask::vTouchBegan(const lw::Touch &touch) {
    _pSliderGame->touchBegan(touch);
}

void SliderTask::vTouchMoved(const lw::Touch &touch) {
    _pSliderGame->touchMoved(touch);
}

void SliderTask::vTouchEnded(const lw::Touch &touch)  {
    _pSliderGame->touchEnded(touch);
    
//    _pSliderGame->clear();
//    //random pick a image
//    srand(time(NULL));
//    int imgIdx = rand() % _fileNames.size();
//    std::string path = IMAGE_FOLDER;
//    path.append(_fileNames[imgIdx]);
//    _pSliderGame->load(path.c_str());
}

void SliderTask::vTouchCanceled(const lw::Touch &touch)  {
    _pSliderGame->touchEnded(touch);
}





