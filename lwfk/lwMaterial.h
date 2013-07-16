#ifndef __LW_MATERIAL_H__
#define __LW_MATERIAL_H__


namespace lw {
    
    class EffectsRes;
    class Mesh;
    class Camera;
    class MaterialInput;
    
    class Material {
    public:
        static Material* create(const char *effectsFile);
        ~Material();
        
        void setFloat(const char *inputName, float value);
        void setVec2(const char *inputName, float x, float y);
        void setVec3(const char *inputName, float x, float y, float z);
        void setVec4(const char *inputName, float x, float y, float z, float w);
        void setTexture(const char *inputName, const char *textureFile, GLint unit);
        
        void draw(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera);
        
        
    private:
        Material(const char *effectsFile, bool &ok);
        
        EffectsRes *_pEffects;
        std::vector<MaterialInput*> _inputs;
    };
    
    
}//namespace lw

#endif //__LW_MATERIAL_H__