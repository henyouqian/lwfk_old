#ifndef __LW_RES_H__
#define __LW_RES_H__

#include "lwLog.h"

namespace lw
{
    
    class ResMgr;
    
    class Res
    {
    public:
        Res(const char *name, ResMgr &mgr);
        virtual ~Res();

        void retain();
        void release();
        const char* getName();

    protected:
        int _refCount;
        std::string _name;
        ResMgr& _mgr;
    };
    
    class ResMgr
    {
    public:
        Res* getRes(const char *name)
        {
            typename std::map<std::string, Res*>::iterator it = _resMap.find(name);
            if (it == _resMap.end())
                return NULL;
            it->second->retain();
            return it->second;
        }
        
        void addRes(Res *pRes)
        {
            const char *name = pRes->getName();
            typename std::map<std::string, Res*>::iterator it = _resMap.find(name);
            if (it == _resMap.end())
                _resMap[name] = pRes;
            else
                lwerror("res allready exists");
        }
        
        void delRes(Res *pRes)
        {
            const char *name = pRes->getName();
            typename std::map<std::string, Res*>::iterator it = _resMap.find(name);
            if (it != _resMap.end())
                _resMap.erase(it);
        }
        
    private:
        std::map<std::string, Res*> _resMap;
        
    };

} //namespace lw


#endif //__LW_RES_H__