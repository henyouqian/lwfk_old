#include "lwPrefix.h"
#include "lwMaterial.h"
#include "lwEffects.h"
#include "lwTexture.h"
#include "lwMesh.h"
#include "lwCamera.h"
#include "lwLog.h"

namespace lw {
    
    class MaterialInput {
    public:
        MaterialInput(GLint location):_location(location) {}
        virtual ~MaterialInput(){};
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) = 0;
        virtual void unuse(){};
        
        GLint _location;
    };
    
    //--------------------------------------
    class MaterialInputFloat : public MaterialInput {
    public:
        MaterialInputFloat(GLint location, float f)
        :MaterialInput(location), _value(f) {}
        
        void set(float f) {
            _value = f;
        }
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            glUniform1f(_location, _value);
        }
        
    private:
        float _value;
    };
    
    //--------------------------------------
    class MaterialInputVec2 : public MaterialInput {
    public:
        MaterialInputVec2(GLint location, float x, float y)
        :MaterialInput(location), _value(x, y) {}
        
        void set(float x, float y) {
            _value.x = x; _value.y = y;
        }
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            glUniform2f(_location, _value.x, _value.y);
        }
        
    private:
        PVRTVec2 _value;
    };
    
    //--------------------------------------
    class MaterialInputVec3 : public MaterialInput {
    public:
        MaterialInputVec3(GLint location, float x, float y, float z)
        :MaterialInput(location), _value(x, y, z) {
        }
        
        void set(float x, float y, float z) {
            _value.x = x; _value.y = y; _value.z = z;
        }
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            glUniform3f(_location, _value.x, _value.y, _value.z);
        }
        
    private:
        PVRTVec3 _value;
    };
    
    //--------------------------------------
    class MaterialInputVec4 : public MaterialInput {
    public:
        MaterialInputVec4(GLint location, float x, float y, float z, float w)
        :MaterialInput(location), _value(x, y, z, w) {}
        
        void set(float x, float y, float z, float w) {
            _value.x = x; _value.y = y; _value.z = z; _value.w = w;
        }
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            glUniform4f(_location, _value.x, _value.y, _value.z, _value.w);
        }
        
    private:
        PVRTVec4 _value;
    };
    
    //--------------------------------------
    class MaterialInputTexture : public MaterialInput {
    public:
        MaterialInputTexture(GLint location, const char *file, GLint unit)
        :MaterialInput(location), _unit(unit){
            _pTexture = TextureRes::create(file);
            assert(_pTexture);
        }
        
        ~MaterialInputTexture() {
            _pTexture->release();
        }
        
        void set(const char *file, GLint unit) {
            TextureRes *pOld = _pTexture;
            _pTexture = TextureRes::create(file);
            assert(_pTexture);
            _unit = unit;
            pOld->release();
        }
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            glActiveTexture(GL_TEXTURE0+_unit);
            glBindTexture(GL_TEXTURE_2D, _pTexture->getGlId());
            glUniform1i(_location, _unit);
        }
        
    private:
        TextureRes *_pTexture;
        GLint _unit;
    };
    
    //--------------------------------------
    //auto inputs below
    class MaterialInputPosition : public MaterialInput {
    public:
        MaterialInputPosition(GLint location)
        :MaterialInput(location) {}
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            if (mesh.position.stride) {
                glEnableVertexAttribArray(_location);
                glVertexAttribPointer(_location, 3, GL_FLOAT, GL_FALSE, mesh.position.stride, mesh.position.ptr);
            }
        }
        
        virtual void unuse(){
            glDisableVertexAttribArray(_location);
        }
    };
    
    //--------------------------------------
    class MaterialInputNormal : public MaterialInput {
    public:
        MaterialInputNormal(GLint location)
        :MaterialInput(location) {}
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            if (mesh.normal.stride) {
                glEnableVertexAttribArray(_location);
                glVertexAttribPointer(_location, 3, GL_FLOAT, GL_FALSE, mesh.normal.stride, mesh.normal.ptr);
            }
        }
        
        virtual void unuse(){
            glDisableVertexAttribArray(_location);
        }
    };
    
    //--------------------------------------
    class MaterialInputUV : public MaterialInput {
    public:
        MaterialInputUV(GLint location, int index) //index: 0->3
        :MaterialInput(location), _index(index){
            assert(index >= 0 && index < 4);
        }
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            const VertexInfo &vi = mesh.uv[_index];
            if (vi.stride) {
                glEnableVertexAttribArray(_location);
                glVertexAttribPointer(_location, 2, GL_FLOAT, GL_FALSE, vi.stride, vi.ptr);
            }
        }
        
        virtual void unuse(){
            glDisableVertexAttribArray(_location);
        }
        
    private:
        int _index;
    };
    
    //--------------------------------------
    class MaterialInputMV : public MaterialInput {
    public:
        MaterialInputMV(GLint location)
        :MaterialInput(location) {}
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            PVRTMat4 m, mView;
            camera.getView(mView);
            m = mView * matWorld;
            glUniformMatrix4fv(_location, 1, false, m.ptr());
        }
    };
    
    //--------------------------------------
    class MaterialInputMVP : public MaterialInput {
    public:
        MaterialInputMVP(GLint location)
        :MaterialInput(location) {}
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            PVRTMat4 m, mViewProj;
            camera.getViewProj(mViewProj);
            m = mViewProj * matWorld;
            glUniformMatrix4fv(_location, 1, false, m.ptr());
        }
    };
    
    //--------------------------------------
    class MaterialInputMVIT : public MaterialInput {
    public:
        MaterialInputMVIT(GLint location)
        :MaterialInput(location) {}
        
        virtual void use(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
            PVRTMat4 m, mView;
            camera.getView(mView);
            m = mView * matWorld;
            PVRTMat3 mvit(m.inverse().transpose());
            glUniformMatrix3fv(_location, 1, false, mvit.ptr());
        }
    };
    
    
    //======================================
    Material* Material::create(const char *effectsFile) {
        bool ok = false;
        Material *pMaterial = new Material(effectsFile, ok);
        if (pMaterial && ok) {
            return pMaterial;
        } else {
            if (pMaterial)
                delete pMaterial;
            return NULL;
        }
    }
    
    Material::Material(const char *effectsFile, bool &ok) {
        assert(effectsFile);
        ok = false;
        
        _pEffects = EffectsRes::create(effectsFile);
        if (_pEffects == NULL) {
            lwerror("EffectsRes::create failed: effectsFile=%s", effectsFile);
            return;
        }
        
        //enumerate attributes and uniforms
        GLuint program = _pEffects->getProgram();
        int total = -1;
        
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &total);
        for(int i=0; i<total; ++i)  {
            int name_len=-1, num=-1;
            GLenum type = GL_ZERO;
            char name[100];
            glGetActiveAttrib( program, GLuint(i), sizeof(name)-1,
                              &name_len, &num, &type, name );
            name[name_len] = 0;
            GLuint location = glGetAttribLocation(program, name);
            MaterialInput *pInput = NULL;
            
            if (strcmp(name, "_position") == 0) {
                pInput = new MaterialInputPosition(location);
            } else if (strcmp(name, "_normal") == 0) {
                pInput = new MaterialInputNormal(location);
                _inputs.push_back(pInput);
            } else if (strcmp(name, "_uv0") == 0) {
                pInput = new MaterialInputUV(location, 0);
                _inputs.push_back(pInput);
            } else if (strcmp(name, "_uv1") == 0) {
                pInput = new MaterialInputUV(location, 1);
                _inputs.push_back(pInput);
            } else if (strcmp(name, "_uv2") == 0) {
                pInput = new MaterialInputUV(location, 2);
                _inputs.push_back(pInput);
            } else if (strcmp(name, "_uv3") == 0) {
                pInput = new MaterialInputUV(location, 3);
            }
            
            if (pInput)
                _inputs.push_back(pInput);
            else
                lwerror("unkown attribute: %s", name);
        }
        
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);
        for(int i=0; i<total; ++i)  {
            int name_len=-1, num=-1;
            GLenum type = GL_ZERO;
            char name[100];
            glGetActiveUniform( program, GLuint(i), sizeof(name)-1,
                               &name_len, &num, &type, name );
            name[name_len] = 0;
            GLuint location = glGetUniformLocation(program, name);
            MaterialInput *pInput = NULL;
            
            if (strcmp(name, "_mv") == 0) {
                pInput = new MaterialInputMV(location);
                _inputs.push_back(pInput);
            } else if (strcmp(name, "_mvp") == 0) {
                pInput = new MaterialInputMVP(location);
                _inputs.push_back(pInput);
            } else if (strcmp(name, "_mvit") == 0) {
                pInput = new MaterialInputMVIT(location);
                _inputs.push_back(pInput);
            }
            
            if (pInput)
                _inputs.push_back(pInput);
            else if (name[0] == '_')
                lwerror("unkown auto uniform: %s", name);
        }
        
        ok = true;
    }
    
    Material::~Material() {
        std::vector<MaterialInput*>::iterator it = _inputs.begin();
        std::vector<MaterialInput*>::iterator itend = _inputs.end();
        for (; it != itend; ++it) {
            delete (*it);
        }
    }
    
    MaterialInput* findInput(std::vector<MaterialInput*>& inputs, int location) {
        std::vector<MaterialInput*>::iterator it = inputs.begin();
        std::vector<MaterialInput*>::iterator itend = inputs.end();
        for (; it != itend; ++it) {
            if ((*it)->_location == location)
                return (*it);
        }
        return NULL;
    }
    
    void Material::setFloat(const char *inputName, float value) {
        int location = _pEffects->getUniformLocation(inputName);
        if (location == -1) {
            lwerror("wrong input name: %s", inputName);
            return;
        }
        
        MaterialInputFloat *pInput = (MaterialInputFloat*)findInput(_inputs, location);
        if ( pInput == NULL) {
            pInput = new MaterialInputFloat(location, value);
            _inputs.push_back(pInput);
        } else {
            pInput->set(value);
        }
    }
    
    void Material::setVec2(const char *inputName, float x, float y) {
        int location = _pEffects->getUniformLocation(inputName);
        if (location == -1) {
            lwerror("wrong input name: %s", inputName);
            return;
        }
        
        MaterialInputVec2 *pInput = (MaterialInputVec2*)findInput(_inputs, location);
        if ( pInput == NULL) {
            pInput = new MaterialInputVec2(location, x, y);
            _inputs.push_back(pInput);
        } else {
            pInput->set(x, y);
        }
    }
    
    void Material::setVec3(const char *inputName, float x, float y, float z) {
        int location = _pEffects->getUniformLocation(inputName);
        if (location == -1) {
            lwerror("wrong input name: %s", inputName);
            return;
        }
        
        MaterialInputVec3 *pInput = (MaterialInputVec3*)findInput(_inputs, location);
        if ( pInput == NULL) {
            pInput = new MaterialInputVec3(location, x, y, z);
            _inputs.push_back(pInput);
        } else {
            pInput->set(x, y, z);
        }
    }
    
    void Material::setVec4(const char *inputName, float x, float y, float z, float w)  {
        int location = _pEffects->getUniformLocation(inputName);
        if (location == -1) {
            lwerror("wrong input name: %s", inputName);
            return;
        }
        
        MaterialInputVec4 *pInput = (MaterialInputVec4*)findInput(_inputs, location);
        if ( pInput == NULL) {
            pInput = new MaterialInputVec4(location, x, y, z, w);
            _inputs.push_back(pInput);
        } else {
            pInput->set(x, y, z, w);
        }
    }
    
    void Material::setTexture(const char *inputName, const char *textureFile, GLint unit) {
        int location = _pEffects->getUniformLocation(inputName);
        if (location == -1) {
            lwerror("wrong input name: %s", inputName);
            return;
        }
        
        MaterialInputTexture *pInput = (MaterialInputTexture*)findInput(_inputs, location);
        if ( pInput == NULL) {
            pInput = new MaterialInputTexture(location, textureFile, unit);
            _inputs.push_back(pInput);
        } else {
            pInput->set(textureFile, unit);
        }
    }
    
    void Material::draw(const lw::Mesh &mesh, const PVRTMat4 &matWorld, const lw::Camera &camera) {
        _pEffects->use();
        std::vector<MaterialInput*>::iterator it = _inputs.begin();
        std::vector<MaterialInput*>::iterator itend = _inputs.end();
        for (; it != itend; ++it) {
            (*it)->use(mesh, matWorld, camera);
        }
        
        glDrawElements(GL_TRIANGLES, mesh.faceCount*3, GL_UNSIGNED_SHORT, 0);
        
        it = _inputs.begin();
        for (; it != itend; ++it) {
            (*it)->unuse();
        }
    }
    
} //namespace lw
