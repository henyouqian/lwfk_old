#include "lwPrefix.h"
#include "lwSprite.h"
#include "lwTexture.h"
#include "lwEffects.h"
#include "lwMaterial.h"
#include "lwMesh.h"
#include "lwRenderState.h"
#include "lwApp.h"
#include "lwLog.h"
#include "tinyxml2/tinyxml2.h"
#include "lwfk/lwCamera.h"
#include "PVR/tools/PVRTResourceFile.h"
#include <dirent.h>
#include <string.h>


namespace lw {
    
    namespace {
        struct AtlasInfo {
            std::string file;
            int u, v, w, h;
        };
        std::map<std::string, AtlasInfo> _atlasMap;
        KeyResMgr _materialResMgr;
    }
    
    Camera _camera;
    
    class SpriteVertexBuffer: public Mesh {
    public:
        SpriteVertexBuffer();
        ~SpriteVertexBuffer();
        
        void collectVetices(SpriteVertex *vertices, int numVertices, Material *pMaterial);
        void flush();
        
        enum {
            VERTICIS_NUM_RESERVE = 8192,
            VERTICIS_NUM_LIMIT = 65536,
        };
        
    private:
        std::vector<SpriteVertex> _vertices;
        Material *_pCurrMaterial;
        int _posLocation;
        int _uvLocation;
        int _mvpMatLocation;
        int _colorLocation;
        int _samplerLocation;
    };
    
    SpriteVertexBuffer::SpriteVertexBuffer() {
        _vertices.reserve(VERTICIS_NUM_RESERVE);
        _pCurrMaterial = NULL;
    }
    
    SpriteVertexBuffer::~SpriteVertexBuffer() {
        
    }
    
    
    void SpriteVertexBuffer::collectVetices(SpriteVertex *vertices, int numVertices, Material *pMaterial) {
        if (pMaterial != _pCurrMaterial || _vertices.size() >= VERTICIS_NUM_LIMIT) {
            flush();
        }
        _pCurrMaterial = pMaterial;
        
        for ( int i = 0; i < numVertices; ++i ) {
            _vertices.push_back(vertices[i]);
        }
    }
    
    void SpriteVertexBuffer::flush() {
        if (_vertices.empty()) {
            return;
        }
        
        //setup mesh
        char* p0 = (char*)&_vertices[0];
        this->position.set(sizeof(SpriteVertex), p0);
        this->uv[0].set(sizeof(SpriteVertex), p0+sizeof(float)*3);
        this->color.set(sizeof(SpriteVertex), p0+sizeof(float)*5);
        this->verticesCount = _vertices.size();
        
        _pCurrMaterial->draw(*this, PVRTMat4::Identity(), _camera, false);
        
        _vertices.clear();
    }
    
    namespace {
        SpriteVertexBuffer* _pvb = NULL;
    }
    
    
    //====================================================
	Sprite* Sprite::createFromFile(const char* textureFile, const char* fxName) {
        assert(textureFile && fxName);
        bool ok = false;
        Sprite* p = new Sprite(textureFile, fxName, false, ok);
        if (p) {
            if (ok) {
                return p;
            } else {
                delete p;
                return NULL;
            }
        }
        return NULL;
    }
    
    Sprite* Sprite::createFromAtlas(const char* key, const char* fxName) {
        bool ok = false;
        Sprite* p = new Sprite(key, fxName, true, ok);
        if (p){
            if (ok) {
                return p;
            } else {
                delete p;
                return NULL;
            }
        }
        return NULL;
    }
    
    namespace {
        void makeFileName2x(std::string& name) {
            int n = name.find('.');
            std::string ext = &name[n];
            name.resize(n);
            name.append("_2x");
            name.append(ext);
        }
    }
    
    Sprite::Sprite(const char *textureFile, const char *fxName, bool fromAtlas, bool &ok) {
        ok = false;
        _ancX = _ancY = 0.f;
        _posX = _posY = 0.f;
        _rotate = 0.f;
        _scaleX = _scaleY = 1.f;
        _color.set(1.0f, 1.0f, 1.0f, 1.0f);
        _needUpdate = true;
        _z = 0.f;
        
        int err = 0;
        if (fromAtlas) {
            err = loadFromAtlas(textureFile, fxName);
        }else{
            err = loadFromFile(textureFile, fxName);
        }
        
        ok = !err;
    }
    
    Sprite::~Sprite() {
        if (_pMaterial && _pMaterial->release() == 0) {
            _materialResMgr.del(_materialKey.c_str());
        }
    }
    
    int Sprite::loadFromFile(const char *textureFile, const char *fxName) {
        assert(textureFile && fxName);
        
        _materialKey = fxName;
        _materialKey.append("/");
        _materialKey.append(textureFile);
        
        _pMaterial = (Material*)_materialResMgr.get(_materialKey.c_str());
        if (_pMaterial == NULL) {
            _pMaterial = Material::create("sprite.lwfx", fxName);
            if (_pMaterial == NULL) {
                lwerror("Material::create failed: sprite.lwfx, %s", fxName);
                return -1;
            }
            _materialResMgr.add(_materialKey.c_str(), _pMaterial);
        }
        _pTexture = _pMaterial->setTexture("input_texture", textureFile, 0);
        if (_pTexture == NULL) {
            lwerror("_pMaterial->setTexture failed: %s", textureFile);
            return -1;
        }
        uvInit();
        
        return 0;
    }
    
    int Sprite::loadFromAtlas(const char *key, const char *fxName) {
        assert(key && fxName);
        
        std::map<std::string, AtlasInfo>::iterator it = _atlasMap.find(key);
        if (it != _atlasMap.end()) {
            AtlasInfo& atlas = it->second;
            int err = loadFromFile(atlas.file.c_str(), fxName);
            if (err) {
                lwerror("loadFromFile error");
                return -1;
            }
            setUV(atlas.u, atlas.v, atlas.w, atlas.h);
            return 0;
        }
        return -1;
    }
    
    void Sprite::setUV(float u, float v, float w, float h) {
        float texW = (float)_pTexture->getWidth();
        float texH = (float)_pTexture->getHeight();
        
        _u = u;
        _v = v;
        _w = w;
        _h = h;
        
        _u1 = u/texW;
		_v1 = v/texH;
        _u2 = (u+_w)/texW;
		_v2 = (v+_h)/texH;
    }
    
    void Sprite::uvInit() {
        _u = _v = 0.f;
        _w = (float)_pTexture->getWidth();
        _h = (float)_pTexture->getHeight();
        _u1 = _v1 = 0.f;
        _u2 = _v2 = 1.f;
    }
    
    void Sprite::getUV(float &u, float &v, float &w, float &h) {
        u = _u; v = _v; w = _w; h = _h;
    }
    
    void Sprite::setAnchor(float x, float y) {
        if (x != _ancX || y != _ancY) {
            _ancX = x; _ancY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getAnchor(float &x, float &y) {
        x = _ancX; y = _ancY;
    }
    
    void Sprite::setPos(float x, float y) {
        if (x != _posX || y != _posY) {
            _posX = x;
            _posY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getPos(float &x, float &y) {
        x = _posX;
        y = _posY;
    }
    
    void Sprite::setRotate(float r) {
        if (_rotate != r) {
            _rotate = r;
            _needUpdate = true;
        }
    }
    
    float Sprite::getRotate() {
        return _rotate;
    }
    
    void Sprite::setScale(float x, float y) {
        if (_scaleX != x || _scaleY != y) {
            _scaleX = x;
            _scaleY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getScale(float &x, float &y) {
        x = _scaleX; y = _scaleY;
    }
    
    void Sprite::setSize(float w, float h) {
        setScale(w/_w, h/_h);
    }
    
    void Sprite::getSize(float &w, float &h) {
        w = _w*_scaleX;
        h = _h*_scaleY;
    }
    
    void Sprite::setColor(const Color& color) {
        _color = color;
    }
    
    void Sprite::setZ(float z) {
        _z = z;
    }
    
    static void setSV(SpriteVertex& sv, float x, float y, float z, float u, float v, Color& color) {
        sv.x = x;
		sv.y = y;
        sv.z = z;
		sv.u = u;
		sv.v = v;
        sv.r = color.r;
        sv.g = color.g;
        sv.b = color.b;
        sv.a = color.a;
    }
    
    void Sprite::draw() {
        if (_needUpdate) {
            update();
        }
        
        //1
		SpriteVertex v[6];
        setSV(v[0], _vertexPos[0].x, _vertexPos[0].y, _z, _u1, _v1, _color);

		//2
        setSV(v[1], _vertexPos[1].x, _vertexPos[1].y, _z, _u1, _v2, _color);
        
		//3
        setSV(v[2], _vertexPos[2].x, _vertexPos[2].y, _z, _u2, _v1, _color);
		
        //3
        setSV(v[3], _vertexPos[2].x, _vertexPos[2].y, _z, _u2, _v1, _color);
		
		//2
        setSV(v[4], _vertexPos[1].x, _vertexPos[1].y, _z, _u1, _v2, _color);

		//4
        setSV(v[5], _vertexPos[3].x, _vertexPos[3].y, _z, _u2, _v2, _color);
        
        _pvb->collectVetices(v, 6, _pMaterial);
    }
    
    void Sprite::update() {
        _needUpdate = false;
        PVRTMat4 m, m1;
        m = PVRTMat4::Identity();
        
        if (_posX or _posY) {
            m1 = PVRTMat4::Translation(_posX, _posY, 0.f);
            m = m * m1;
        }
		if (_rotate != 0.f) {
			m1 = PVRTMat4::RotationZ(_rotate);
			m = m * m1;
		}
		if (_scaleX != 1.f || _scaleY != 1.f) {
            m1 = PVRTMat4::Scale(_scaleX, _scaleY, 1.f);
			m = m * m1;
		}
        
        float posX1 = -_ancX;
		float posY1 = -_ancY;
		float posX2 = posX1 + _w;
		float posY2 = posY1 + _h;
        
        PVRTVec3 pt[4];
		pt[0].x = posX1;
        pt[0].y = posY1;
        pt[0].z = _z;
        pt[1].x = posX1;
        pt[1].y = posY2;
        pt[1].z = _z;
        pt[2].x = posX2;
        pt[2].y = posY1;
        pt[2].z = _z;
		pt[3].x = posX2;
        pt[3].y = posY2;
        pt[3].z = _z;
        
        PVRTVec3 ptt[4];
		PVRTTransformArray(ptt, pt, 4, &m, f2vt(1.0));
        
        _vertexPos[0].x = ptt[0].x;
		_vertexPos[0].y = -ptt[0].y;
        
        _vertexPos[1].x = ptt[1].x;
		_vertexPos[1].y = -ptt[1].y;
        
        _vertexPos[2].x = ptt[2].x;
		_vertexPos[2].y = -ptt[2].y;
        
        _vertexPos[3].x = ptt[3].x;
		_vertexPos[3].y = -ptt[3].y;
    }
    
//    GLuint Sprite::getGlId(){
//        return _pTextureRes->getGlId();
//    }

    
    //===============================================
    void loadAtlasConf();
    
    void Sprite::init() {
        _pvb = new SpriteVertexBuffer();
        
        PVRTVec2 ss = screenSize();
        _camera.lookat(PVRTVec3(0.f, 0.f, 100.f), PVRTVec3(0.f, 0.f, 0.f), PVRTVec3(0.f, 1.f, 0.f));
        _camera.ortho(ss.x, ss.y, 0, 200.f);
        
        loadAtlasConf();
    }
    
    void Sprite::quit() {
        if (_pvb) {
            delete _pvb;
            _pvb = NULL;
        }
    }
    
    void Sprite::flush() {
        _pvb->flush();
    }
    
    
    void addAtlas(const char *file) {
        assert(file);
        tinyxml2::XMLDocument doc;
        CPVRTResourceFile resFile(file);
        if (resFile.IsOpen()) {
            tinyxml2::XMLError err = doc.Parse((const char*)resFile.DataPtr(), resFile.Size());
            assert(err == tinyxml2::XML_SUCCESS);
            const tinyxml2::XMLElement *pElem = doc.RootElement();
            pElem = pElem->FirstChildElement("image");
            while ( pElem ){
                const char *name = pElem->Attribute("name");
                AtlasInfo atlas;
                atlas.file = pElem->Attribute("file");
                pElem->QueryIntAttribute("x", &atlas.u);
                pElem->QueryIntAttribute("y", &atlas.v);
                pElem->QueryIntAttribute("w", &atlas.w);
                pElem->QueryIntAttribute("h", &atlas.h);
                _atlasMap[name] = atlas;
                
                pElem = pElem->NextSiblingElement();
            }
        } else {
            lwerror("file open error: %s", file);
        }
    }
    
    void loadAtlasConf() {
        const char * path = CPVRTResourceFile::GetReadPath().c_str();
        DIR *pDir = opendir(path);
        struct dirent *pDirent;
        while (1) {
            pDirent = readdir(pDir);
            if (pDirent == NULL)
                break;
            const char *p = strrchr(pDirent->d_name, '.');
            if (p && strcmp(p, ".atlas") == 0) {
                addAtlas(pDirent->d_name);
                lwinfo("%s", pDirent->d_name);
            }
        }
        closedir(pDir);
    }
    
} //namespace lw
