#include "lwPrefix.h"
#include "lwStage.h"

namespace lw {
    
struct StageMgr
{
    Stage* running;
    Stage* incoming;
};

static StageMgr *_pStageMgr = NULL;

void Stage::create()
{
    assert(!_pStageMgr);
    _pStageMgr = new StageMgr();
    _pStageMgr->running = NULL;
    _pStageMgr->incoming = NULL;
}

void Stage::destroy()
{
    assert(_pStageMgr);
    delete _pStageMgr;
    _pStageMgr = NULL;
}

void Stage::update()
{
    if (_pStageMgr->incoming) {
        if (_pStageMgr->running) {
            _pStageMgr->running->vDisactive();
        }
        _pStageMgr->running = _pStageMgr->incoming;
        _pStageMgr->running->vActive();
        _pStageMgr->incoming = NULL;
    }
    if (_pStageMgr->running)
        _pStageMgr->running->vUpdate();
}

void Stage::draw()
{
    if (_pStageMgr->running)
        _pStageMgr->running->vDraw();
}
    
//======================================
Stage::Stage()
{
    
}

Stage::~Stage()
{
    
}
    
void Stage::active()
{
    _pStageMgr->incoming = this;
}
    

} //namespace lw
