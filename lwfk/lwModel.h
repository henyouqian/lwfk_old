#ifndef __LW_MODEL_H__
#define __LW_MODEL_H__

#include "lwMesh.h"

namespace lw {
    
    class Camera;
    class PodRes;
    class Material;
    
    class Model {
    public:
        static Model* create(const char *file);
        ~Model();
        
        void setWorldMatrix(const PVRTMat4 &matWorld);
        void draw(const lw::Camera &camera);
        
    private:
        Model(const char *file, bool &ok);
        
        PodRes *_pPodRes;
        GLuint* _vbos;
        GLuint* _indexVbos;
        int _vbosNum;
        PVRTMat4 _worldMatrix;
        
        std::vector<lw::Mesh> _meshes;
        std::vector<lw::Material*> _materials;
    };
    
    
}//namespace lw

#endif //__LW_MODEL_H__