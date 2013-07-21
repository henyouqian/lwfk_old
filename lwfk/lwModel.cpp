#include "lwPrefix.h"
#include "lwModel.h"
#include "lwMaterial.h"
#include "lwCamera.h"
#include "lwRes.h"
#include "lwLog.h"

#include "PVR/tools/PVRTModelPOD.h"
#include "PVR/tools/PVRTResourceFile.h"
#include "tinyxml2/tinyxml2.h"

namespace lw {
    
    class PodRes : public Res {
    public:
        static PodRes* create(const char *file);
        CPVRTModelPOD& getPOD();
        
    private:
        PodRes(const char *file, bool &ok);
        ~PodRes();
        
        CPVRTModelPOD _pod;
        std::string _fileName;
    };
    
    KeyResMgr _resMgr;
    
    PodRes* PodRes::create(const char *file) {
        assert(file);
        PodRes *pRes = (PodRes*)_resMgr.get(file);
        if (pRes)
            return pRes;
        
        bool ok = false;
        pRes = new PodRes(file, ok);
        if (pRes && !ok) {
            lwerror("new PodRes failed");
            delete pRes;
            return NULL;
        }
        _resMgr.add(file, pRes);
        return pRes;
    }
    
    PodRes::PodRes(const char *file, bool &ok) {
        assert(file);
        ok = false;
        _fileName = file;
        
        CPVRTResourceFile resFile(file);
        if (resFile.IsOpen()) {
            if(_pod.ReadFromMemory((char*)resFile.DataPtr(), resFile.Size())) {
                lwerror("Failed to load pod file. file=%s", file);
                assert(0);
                return;
            }
        }
        ok = true;
    }
    
    PodRes::~PodRes() {
        _resMgr.del(_fileName.c_str());
    }
    
    CPVRTModelPOD& PodRes::getPOD() {
        return _pod;
    }
    
    //================================================
    Model* Model::create(const char *file) {
        bool ok = false;
        Model *pModel = new Model(file, ok);
        if (pModel && ok) {
            return pModel;
        } else {
            if (pModel)
                delete pModel;
            return NULL;
        }
    }
    
    Model::Model(const char *file, bool &ok) {
        ok = false;
        assert(file);
        
        _worldMatrix = PVRTMat4::Identity();
        
        //lwmdl file
        tinyxml2::XMLDocument doc;
        CPVRTResourceFile resFile(file);
        if (!resFile.IsOpen()) {
            lwerror("open lwmdl file failed: file=%s", file);
            return;
        }
        doc.Parse((const char*)resFile.DataPtr(), resFile.Size());
        const tinyxml2::XMLElement *pElemLwmdl = doc.RootElement();
        const char *podFile = pElemLwmdl->Attribute("pod");
        assert(podFile);
        
        //vbos
        _vbos = _indexVbos = NULL;
        
        _pPodRes = PodRes::create(podFile);
        assert(_pPodRes);
        
        CPVRTModelPOD &pod = _pPodRes->getPOD();
        
        if (!pod.pMesh[0].pInterleaved) {
            lwerror("requires the pod data to be interleaved");
            assert(0);
            return;
        }
        
        _vbosNum = pod.nNumMesh;
        _vbos = new GLuint[_vbosNum];
        _indexVbos = new GLuint[_vbosNum];
        
        glGenBuffers(pod.nNumMesh, _vbos);
        for (unsigned int i = 0; i < pod.nNumMesh; ++i)
        {
            // Load vertex data into buffer object
            SPODMesh& podMesh = pod.pMesh[i];
            unsigned int uiSize = podMesh.nNumVertex * podMesh.sVertex.nStride;
            glBindBuffer(GL_ARRAY_BUFFER, _vbos[i]);
            glBufferData(GL_ARRAY_BUFFER, uiSize, podMesh.pInterleaved, GL_STATIC_DRAW);
            
            // Load index data into buffer object if available
            _indexVbos[i] = 0;
            if (podMesh.sFaces.pData)
            {
                glGenBuffers(1, &_indexVbos[i]);
                uiSize = PVRTModelPODCountIndices(podMesh) * sizeof(GLushort);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVbos[i]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, uiSize, podMesh.sFaces.pData, GL_STATIC_DRAW);
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        //meshes
        const tinyxml2::XMLElement *pElemMesh = pElemLwmdl->FirstChildElement("mesh");
        while ( pElemMesh ){
            //mesh
            int meshId = pElemMesh->IntAttribute("id");
            assert(meshId < pod.nNumMesh );
            SPODMesh* pMesh = pod.pMesh + meshId;
            
            Mesh mesh;
            mesh.position.set(pMesh->sVertex.nStride, pMesh->sVertex.pData);
            mesh.normal.set(pMesh->sNormals.nStride, pMesh->sNormals.pData);
            
            int maxuv = std::max((int)pMesh->nNumUVW, (int)UV_SIZE);
            for (int i = 0; i < maxuv; ++i) {
                mesh.uv[i].set(pMesh->psUVW[i].nStride, pMesh->psUVW[i].pData);
            }
            mesh.verticesCount = pMesh->nNumFaces*3;
            _meshes.push_back(mesh);
            
            //material
            const char *fxFile = pElemMesh->Attribute("fxFile");
            const char *fxName = pElemMesh->Attribute("fxName");
            assert(fxFile && fxName);
            Material *pMaterial = Material::create(fxFile, fxName);
            assert(pMaterial);
            
            
            const tinyxml2::XMLElement *pElemInput = pElemMesh->FirstChildElement("input");
            while ( pElemInput ) {
                const char *name = pElemInput->Attribute("name");
                const char *type = pElemInput->Attribute("type");
                const char *value = pElemInput->Attribute("value");
                assert(name && type && value);
                
                if (strcmp(type, "FLOAT") == 0) {
                    float f;
                    sscanf(value, "%f", &f);
                    pMaterial->setFloat(name, f);
                } else if (strcmp(type, "VEC2") == 0) {
                    float x, y;
                    sscanf(value, "%f,%f", &x, &y);
                    pMaterial->setVec2(name, x, y);
                } else if (strcmp(type, "VEC3") == 0) {
                    float x, y, z;
                    sscanf(value, "%f,%f,%f", &x, &y, &z);
                    pMaterial->setVec3(name, x, y, z);
                } else if (strcmp(type, "VEC4") == 0) {
                    float x, y, z, w;
                    sscanf(value, "%f,%f,%f,%f", &x, &y, &z, &w);
                    pMaterial->setVec4(name, x, y, z, w);
                } else if (strncmp(type, "TEXTURE", 7) == 0) {
                    int unit = 0;
                    sscanf(type, "TEXTURE%d", &unit);
                    pMaterial->setTexture(name, value, unit);
                }
                
                pElemInput = pElemInput->NextSiblingElement("input");
            }
            _materials.push_back(pMaterial);
            
            pElemMesh = pElemMesh->NextSiblingElement("mesh");
        }
        
        ok = true;
    }
    
    Model::~Model() {
        _pPodRes->release();
        if ( _vbos )
            glDeleteBuffers(_vbosNum, _vbos);
        delete [] _vbos;
        if ( _indexVbos )
            glDeleteBuffers(_vbosNum, _indexVbos);
        delete [] _indexVbos;
        
        std::vector<lw::Material*>::iterator it = _materials.begin();
        std::vector<lw::Material*>::iterator itend = _materials.end();
        for (;it != itend; ++it) {
            (*it)->release();
        }
    }
    
    void Model::setWorldMatrix(const PVRTMat4 &matWorld) {
        _worldMatrix = matWorld;
    }
    
    void Model::draw(const lw::Camera &camera) {
        CPVRTModelPOD &pod = _pPodRes->getPOD();
        
        for (unsigned int i = 0; i < pod.nNumMeshNode; ++i)
        {
            SPODNode& node = pod.pNode[i];
            
            // Get the node model matrix
            PVRTMat4 mWorld;
            mWorld = pod.GetWorldMatrix(node);
            
            int meshIdx = node.nIdx;
            if ( meshIdx < _meshes.size() ){
                assert(_vbos[meshIdx] && _indexVbos[meshIdx]);
                glBindBuffer(GL_ARRAY_BUFFER, _vbos[meshIdx]);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVbos[meshIdx]);

                _materials[meshIdx]->draw(_meshes[meshIdx], mWorld * _worldMatrix, camera, true);
                
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }
    }
    
    
    
} //namespace lw
