#ifndef __LW_RES_H__
#define __LW_RES_H__

namespace lw {
    
	class Res {
	public:
		Res(): _refCount(1) {}
		virtual ~Res() {};
        
		void retain() {
			++_refCount;
		}
		int release() {
			--_refCount; if ( _refCount == 0 ) delete this;
            return _refCount;
		}
        int refCount() {
            return _refCount;
        }
        
	private:
		int _refCount;
	};
    
    class KeyResMgr {
    public:        
        int add(const char *key, Res *pRes);
        Res* get(const char *key);
        int del(const char *key);
        
    private:
        std::map<std::string, Res*> _resMap;
    };
    
} //namespace lw


#endif //__LW_RES_H__