#ifndef __LW_MATERIAL_H__
#define __LW_MATERIAL_H__


namespace lw {
    
    class EffectsRes;
    class Mesh;
    class Camera;
    class MaterialInput;
    
    class Material {
    public:
        static Material* create(const char *fxFile, const char *fxName);
        ~Material();
        
        void setFloat(const char *inputName, float value);
        void setVec2(const char *inputName, float x, float y);
        void setVec3(const char *inputName, float x, float y, float z);
        void setVec4(const char *inputName, float x, float y, float z, float w);
        void setTexture(const char *inputName, const char *textureFile, GLint unit);
        
        void draw(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera);
        
        
    private:
        Material(const char *effectsFile, const char* fxName, bool &ok);
        
        EffectsRes *_pEffects;
        std::string _fxName;
        std::vector<MaterialInput*> _attribInputs;
        std::vector<MaterialInput*> _uniformInputs;
    };
    
    
}//namespace lw

#endif //__LW_MATERIAL_H__