#include "lwPrefix.h"
#include "lwTexture.h"
#include "lwLog.h"
#include "soil/SOIL.h"
#include <map>
#include "PVR/tools/PVRTResourceFile.h"

namespace lw {

    namespace {
        KeyResMgr _resMgr;
    }
	

	TextureRes::TextureRes(const char* fileName, bool& ok)
    :_glId(-1) {
        ok = false;
		assert(fileName);
		_fileName = fileName;

		size_t len = strlen(fileName);
		if ( len < 4 ) {
			lwerror("texture file name too short: %s", fileName);
			_glId = -1;
			return;
		}
    
        CPVRTResourceFile resFile(fileName);
        if (resFile.IsOpen()) {
            if (loadAndCreateOgl((const unsigned char*)resFile.DataPtr(), resFile.Size())) {
                lwerror("Failed to load texture: %s", fileName);
            }
        } else {
            lwerror("texture file is not exist: %s", fileName);
            return;
        }
        ok = true;
	}

	int TextureRes::loadAndCreateOgl(const unsigned char* buf, int buflen) {
		unsigned char* pImgData = SOIL_load_image_from_memory(buf, buflen, &_w, &_h, &_numChannels, SOIL_LOAD_AUTO);
        assert(pImgData);
		_glId = SOIL_internal_create_OGL_texture(pImgData, _w, _h, _numChannels,
			SOIL_CREATE_NEW_ID, 0,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE);
		
		SOIL_free_image_data(pImgData);
		
		if ( _glId == 0 ){
            _glId = -1;
            return -1;
		}
        return 0;
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}

	TextureRes::~TextureRes() {
		if ( _glId != -1 ){
			glDeleteTextures(1, &_glId);
		}
        _resMgr.del(_fileName.c_str());
	}
	TextureRes* TextureRes::create(const char* fileName){
		assert(fileName);
        
        TextureRes *pRes = (TextureRes*)_resMgr.get(fileName);
        if (pRes) {
            return pRes;
        } else {
            bool ok = false;
            TextureRes* p = new TextureRes(fileName, ok);
			if ( p && ok  ){
                _resMgr.add(fileName, p);
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