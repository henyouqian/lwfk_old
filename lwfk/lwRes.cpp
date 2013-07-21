#include "lwPrefix.h"
#include "lwRes.h"
#include "lwLog.h"

namespace lw {
    
    int KeyResMgr::add(const char *key, Res *pRes) {
        assert(key);
        std::map<std::string, Res*>::iterator it = _resMap.find(key);
        if (it != _resMap.end()) {
            lwerror("Key already exists: %s", key);
            return -1;
        }
        _resMap[key] = pRes;
        return 0;
    }
    
    Res* KeyResMgr::get(const char *key) {
        assert(key);
        std::map<std::string, Res*>::iterator it = _resMap.find(key);
        if (it == _resMap.end()) {
            return NULL;
        }
        it->second->retain();
        return it->second;
    }
    
    int KeyResMgr::del(const char *key) {
        assert(key);
        std::map<std::string, Res*>::iterator it = _resMap.find(key);
        if (it == _resMap.end()) {
            lwerror("Key not found: %s", key);
            return -1;
        }
        _resMap.erase(it);
        return 0;
    }
    
}

