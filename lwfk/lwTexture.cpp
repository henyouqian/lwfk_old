#include "lwPrefix.h"
#include "lwTexture.h"
#include "lwFileSys.h"
#include "lwLog.h"
#include "soil/SOIL.h"
#include <map>

namespace lw {

    namespace {
        ResMgr _resMgr;
    }
	

	TextureRes::TextureRes(const char* fileName, bool& ok):Res(fileName, _resMgr), _glId(-1) {
        ok = false;
		assert(fileName);
		_fileName = fileName;

		size_t len = strlen(fileName);
		if ( len < 4 ) {
			lwerror("texture file name too short: filepath = " << (const char*)_f(fileName));
			_glId = -1;
			return;
		}
		_f fpath(fileName);
        if (fpath) {
            loadAndCreateOgl(fpath);
        } else {
            lwerror("texture file is not exist: " << fileName);
            return;
        }
        ok = true;
	}

	void TextureRes::loadAndCreateOgl(const char* path) {
		unsigned char* pImgData = SOIL_load_image(path, &_w, &_h, &_numChannels, SOIL_LOAD_AUTO);
		_glId = SOIL_internal_create_OGL_texture(pImgData, _w, _h, _numChannels,
			SOIL_CREATE_NEW_ID, 0,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE);
		
		SOIL_free_image_data(pImgData);
		
		if ( _glId == 0 ){
			lwerror("Failed to load texture: path=" << path);
			_glId = -1;
		}
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}

	TextureRes::~TextureRes() {
		if ( _glId != -1 ){
			glDeleteTextures(1, &_glId);
		}
	}
	TextureRes* TextureRes::create(const char* fileName){
		assert(fileName);
		std::string strFileName = fileName;
        
        TextureRes *pRes = (TextureRes*)_resMgr.getRes(fileName);
        if (pRes) {
            return pRes;
        } else {
            bool ok = false;
            TextureRes* p = new TextureRes(fileName, ok);
			if ( p && ok  ){
				return p;
			}else if (p){
				delete p;
				return NULL;
			}
        }
        return NULL;
	}
    
    GLuint TextureRes::getGlId(){
        return _glId;
    }
    
    const char* TextureRes::getFileName(){
        return _fileName.c_str();
    }
    
    int TextureRes::getWidth(){
        return _w;
    }
    
    int TextureRes::getHeight(){
        return _h;
    }
    
    int TextureRes::getChannelNum(){
        return _numChannels;
    }

} //namespace lw