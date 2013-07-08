#include "lwPrefix.h"
#include "lwRes.h"

namespace lw {
    
    Res::Res(const char *name, ResMgr &mgr): _refCount(1), _name(name), _mgr(mgr)
    {
        _mgr.addRes(this);
    }
    
    Res::~Res() {
        _mgr.delRes(this);
    };
    
    void Res::retain()
    {
        ++_refCount;
    }
    
    void Res::release()
    {
        --_refCount;
        if ( _refCount == 0 )
            delete this;
    }
    
    const char* Res::getName()
    {
        return _name.c_str();
    }
    
}

