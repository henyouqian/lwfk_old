#include "lwPrefix.h"
#include "lwFileSys.h"
#include "lwApp.h"

namespace lw {
	const char* getDocDir(){
		return "doc";
	}
    
    _f::_f(const char* filename){
        _path = getReadPath();
        _path.append(filename);
    }
    
    _f::operator const char*(){
        return _path.c_str();
    }
    _f::operator bool(){
        return !_path.empty();
    }
    
    _fdoc::_fdoc(const char* filename){
        _path = getDocDir();
        _path.append("/");
        _path.append(filename);
    }
    
    _fdoc::operator const char*(){
        return _path.c_str();
    }
}

