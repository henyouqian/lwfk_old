#ifndef __LW_SOUND_H__
#define __LW_SOUND_H__

#import "ALUT/alut.h"

namespace lw {
    
    void soundInit();
    void soundQuit();
    
    class SoundBufferRes;
    
    class Sound{
    public:
        Sound(const char *file);
        virtual ~Sound();
        void play();
        void stop();
        
    private:
        ALuint _source;
        SoundBufferRes *_pBufRes;
    };
    
}

#endif //__LW_THREAD_H__