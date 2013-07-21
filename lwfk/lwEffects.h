#ifndef __LW_EFFECTS_H__
#define __LW_EFFECTS_H__

#include "lwfk/lwRes.h"

namespace lw
{
    
    class EffectFx;
    
    class EffectsRes : public Res
    {
    public:
        
        /*
         _position,
         _normal,
         _binormal,
         _tangent,
         _uv0,
         _uv1,
         _uv2,
         _uv3,
         _mv,
         _mvp,
         _mvit,
         
         */
        
        
        static EffectsRes* create(const char *file);
        GLuint getProgram(const char *fxName);
        int getUniformLocation(const char* uniform, const char *fxName);
        
        void use(const char *fxName);
        bool checkFxName(const char *fxName);
        
    private:
        EffectsRes(const char *file, bool &ok);
        ~EffectsRes();
        
        std::vector<EffectFx*> _fxs;
        std::string _fileName;
    };
    
    
}//namespace lw

#endif //__LW_EFFECTS_H__