#ifndef __LW_MATERIAL_H__
#define __LW_MATERIAL_H__

#include "lwfk/lwRes.h"

// Material = instance of fx = fx + inputs(aka attributes)

namespace lw {
    
    class EffectsRes;
    class Mesh;
    class Camera;
    class MaterialInput;
    
    class Material : public Res {
    public:
        static Material* create(const char *fxFile, const char *fxName);
        
        void setFloat(const char *inputName, float value);
        void setVec2(const char *inputName, float x, float y);
        void setVec3(const char *inputName, float x, float y, float z);
        void setVec4(const char *inputName, float x, float y, float z, float w);
        void setTexture(const char *inputName, const char *textureFile, GLint unit);
        
        void draw(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera, bool useIndex);
        
        
    private:
        Material(const char *effectsFile, const char* fxName, bool &ok);
        ~Material();
        
        EffectsRes *_pEffects;
        std::string _fxName;
        std::vector<MaterialInput*> _attribInputs;
        std::vector<MaterialInput*> _uniformInputs;
    };
    
    
}//namespace lw

#endif //__LW_MATERIAL_H__