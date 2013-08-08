#include "lwPrefix.h"
#include "lwSound.h"
#include "lwLog.h"
#include "lwRes.h"
#include "PVR/tools/PVRTResourceFile.h"

namespace lw {

    void soundInit() {
        alutInit (NULL, NULL);
    }
    
    void soundQuit() {
        alutExit();
    }
    
    namespace { KeyResMgr _soundBufferResMgr; }
    
    class SoundBufferRes : public Res {
    public:
        static SoundBufferRes* create(const char *filename) {
            assert(filename);
            
            SoundBufferRes *pRes = (SoundBufferRes*)_soundBufferResMgr.get(filename);
            if (pRes) {
                return pRes;
            } else {
                bool ok = false;
                SoundBufferRes* p = new SoundBufferRes(filename, ok);
                if ( p && ok  ){
                    return p;
                }else if (p){
                    delete p;
                    return NULL;
                }
            }
            return NULL;
        }
        
        ALuint getBuffer() {
            return _buffer;
        }
        
    private:
        SoundBufferRes(const char *filename, bool &ok)
        :_buffer(0){
            assert(filename);
            ok = false;
            
            CPVRTResourceFile resFile(filename);
            if (!resFile.IsOpen()) {
                lwerror("sound file is not exist: %s", filename);
                return;
            }
            
            _buffer = alutCreateBufferFromFileImage(resFile.DataPtr(), resFile.Size());
            if (_buffer == AL_NONE) {
                lwerror("sound buffer create failed: %s", filename);
                return;
            }
            
            ok = true;
            _filename = filename;
            _soundBufferResMgr.add(filename, this);
        };
    
        ~SoundBufferRes() {
            if (_buffer)
                alDeleteBuffers(1, &_buffer);
            if (!_filename.empty())
                _soundBufferResMgr.del(_filename.c_str());
        }
        
    private:
        std::string _filename;
        ALuint _buffer;
    };
    
    
    Sound::Sound(const char *file) {
        _pBufRes = SoundBufferRes::create(file);
        if (_pBufRes == NULL) {
            _pBufRes = SoundBufferRes::create("default.wav");
            assert(_pBufRes);
        }
        
        alGenSources(1, &_source);
        ALenum e = alGetError();
        if (e != AL_NO_ERROR) {
            lwerror("alGenSources failed: error=%d", e);
            return;
        }
        alSourcei(_source, AL_BUFFER, _pBufRes->getBuffer());
    }
    
    Sound::~Sound() {
        _pBufRes->release();
        alDeleteSources(1, &_source);
    }
    
    void Sound::play() {
        alSourcePlay(_source);
    }
    
    void Sound::stop() {
        alSourceStop(_source);
    }
}
