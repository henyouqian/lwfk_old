#include "lwFileSys.h"

namespace lw {
	const char* getDocDir(){
		static std::string docDir;
		if ( docDir.empty() ){
			NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
			NSString* documentsDirectory = [paths objectAtIndex:0];
			docDir = [documentsDirectory UTF8String];
		}
		return docDir.c_str();
	}
    
    _f::_f(const char* filename){
        NSString* str = [NSString stringWithUTF8String:filename];
        NSString* path = [[NSBundle mainBundle] pathForResource:str ofType:nil];
        if (path){
            _path = [path UTF8String];
        }
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

