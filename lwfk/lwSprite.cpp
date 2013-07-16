#include "lwPrefix.h"
#include "lwSprite.h"
#include "lwTexture.h"
#include "lwEffects.h"
#include "lwApp.h"
#include "lwFileSys.h"
#include "tinyxml2/tinyxml2.h"
#include "lwfk/lwCamera.h"


namespace lw {
    
    namespace {
        struct AtlasInfo{
            std::string file;
            int u, v, w, h;
        };
        std::map<std::string, AtlasInfo> _atlasMap;
    }
    
    Camera _camera;
    
    class SpriteVertexBuffer{
    public:
        SpriteVertexBuffer();
        ~SpriteVertexBuffer();
        void collectVetices(SpriteVertex *vertices, int numVertices, Color &color, BlendMode blendMode, GLuint textureId);
        void flush();
        
        enum{
            VERTICIS_NUM_RESERVE = 8192,
            VERTICIS_NUM_LIMIT = 65536,
        };
        
    private:
        std::vector<SpriteVertex> _vertices;
        Color _currColor;
        BlendMode _currBlendMode;
        GLuint _currTextureId;
        EffectsRes *_pEffects;
        int _posLocation;
        int _uvLocation;
        int _mvpMatLocation;
        int _colorLocation;
        int _samplerLocation;
    };
    
    SpriteVertexBuffer::SpriteVertexBuffer(){
        _vertices.reserve(VERTICIS_NUM_RESERVE);
        _pEffects = EffectsRes::create("sprite.lwfx");
        _posLocation = _pEffects->getLocationFromSemantic(EffectsRes::POSITION);
        _uvLocation = _pEffects->getLocationFromSemantic(EffectsRes::UV0);
        _mvpMatLocation = _pEffects->getLocationFromSemantic(EffectsRes::WORLDVIEWPROJ);
        _colorLocation = _pEffects->getUniformLocation("input_color");
        _samplerLocation = _pEffects->getUniformLocation("input_texture");
    }
    
    SpriteVertexBuffer::~SpriteVertexBuffer(){
        if ( _pEffects ){
            _pEffects->release();
        }
    }
    
    void SpriteVertexBuffer::collectVetices(SpriteVertex *vertices, int numVertices, Color &color, BlendMode blendMode, GLuint textureId){
        if ( !_vertices.empty() && 
            ( color != _currColor 
             || blendMode != _currBlendMode 
             || textureId != _currTextureId 
             || _vertices.size() >= VERTICIS_NUM_LIMIT-3 ) )
        {
            flush();
        }
        _currColor = color;
        _currBlendMode = blendMode;
        _currTextureId = textureId;
        
        for ( int i = 0; i < numVertices; ++i ) {
            _vertices.push_back(vertices[i]);
        }
    }
    
    void SpriteVertexBuffer::flush(){
        if ( _vertices.empty() ){
            return;
        }
        _pEffects->use();
        PVRTMat4 m;
        _camera.getViewProj(m);
        glUniformMatrix4fv(_mvpMatLocation, 1, false, m.f);
        glUniform4f(_colorLocation, _currColor.r, _currColor.g, _currColor.b, _currColor.a);
        
        //glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _currTextureId);
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glUniform1i(_samplerLocation, 0);
        glEnable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
        if ( _currBlendMode == BLEND_NONE ){
			glDisable(GL_BLEND);
		}else{
			glEnable(GL_BLEND);
			if ( _currBlendMode == BLEND_NORMAL ){
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}else if ( _currBlendMode == BLEND_ADD ){
				glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			}else if ( _currBlendMode == BLEND_MUL ){
                glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            }
		}
        
        const char* p = (char*)&_vertices[0];
        glEnableVertexAttribArray(_posLocation);
        glVertexAttribPointer(_posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), p);
        glEnableVertexAttribArray(_uvLocation);
        glVertexAttribPointer(_uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), p+3*sizeof(float));
        
        glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
        
        _vertices.clear();
    }
    
    namespace {
        SpriteVertexBuffer* _pvb = NULL;
    }
    
    
    //====================================================
	Sprite* Sprite::createFromFile(const char *file){
        bool ok = false;
        Sprite* p = new Sprite(file, false, ok);
        if ( p ){
            if ( ok ){
                return p;
            }else{
                delete p;
                return NULL;
            }
        }
        return NULL;
    }
    
    Sprite* Sprite::createFromAtlas(const char* key){
        bool ok = false;
        Sprite* p = new Sprite(key, true, ok);
        if ( p ){
            if ( ok ){
                return p;
            }else{
                delete p;
                return NULL;
            }
        }
        return NULL;
        return NULL;
    }
    
    namespace{
        void makeFileName2x(std::string& name){
            int n = name.find('.');
            std::string ext = &name[n];
            name.resize(n);
            name.append("_2x");
            name.append(ext);
        }
    }
    
    Sprite::Sprite(const char *file, bool fromAtlas, bool &ok){
        ok = false;
        _ancX = _ancY = 0.f;
        _posX = _posY = 0.f;
        _rotate = 0.f;
        _scaleX = _scaleY = 1.f;
        _color.set(1.0f, 1.0f, 1.0f, 1.0f);
        _blendMode = BLEND_NORMAL;
        _needUpdate = true;
        _pTextureRes = NULL;
        _z = 0.f;
        
        if ( fromAtlas ){
            loadFromAtlas(file);
        }else{
            loadFromFile(file);
        }
        
        if ( _pTextureRes ){
            ok = true;
        }
    }
    
    Sprite::~Sprite(){
        if ( _pTextureRes ){
			_pTextureRes->release();
		}
    }
    
    void Sprite::loadFromFile(const char *file){
        _pTextureRes = TextureRes::create(file);
        if ( _pTextureRes ){
            uvInit();
        }
    }
    
    void Sprite::loadFromAtlas(const char *key){
        std::map<std::string, AtlasInfo>::iterator it = _atlasMap.find(key);
        if ( it != _atlasMap.end() ){
            AtlasInfo& atlas = it->second;
            _pTextureRes = TextureRes::create(atlas.file.c_str());
            if ( _pTextureRes ){
                setUV(atlas.u, atlas.v, atlas.w, atlas.h);
            }
        }
    }
    
    void Sprite::setUV(float u, float v, float w, float h){
        float texW = (float)_pTextureRes->getWidth();
        float texH = (float)_pTextureRes->getHeight();
        
        _u = u;
        _v = v;
        _w = w;
        _h = h;
        
        _u1 = u/texW;
		_v1 = v/texH;
        _u2 = (u+_w)/texW;
		_v2 = (v+_h)/texH;
    }
    
    void Sprite::uvInit(){
        _u = _v = 0.f;
        _w = (float)_pTextureRes->getWidth();
        _h = (float)_pTextureRes->getHeight();
        _u1 = _v1 = 0.f;
        _u2 = _v2 = 1.f;
    }
    
    void Sprite::getUV(float &u, float &v, float &w, float &h){
        u = _u; v = _v; w = _w; h = _h;
    }
    
    void Sprite::setAnchor(float x, float y){
        if ( x != _ancX || y != _ancY ){
            _ancX = x; _ancY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getAnchor(float &x, float &y){
        x = _ancX; y = _ancY;
    }
    
    void Sprite::setPos(float x, float y){
        if ( x != _posX || y != _posY ){
            _posX = x;
            _posY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getPos(float &x, float &y){
        x = _posX;
        y = _posY;
    }
    
    void Sprite::setRotate(float r){
        if ( _rotate != r ){
            _rotate = r;
            _needUpdate = true;
        }
    }
    
    float Sprite::getRotate(){
        return _rotate;
    }
    
    void Sprite::setScale(float x, float y){
        if ( _scaleX != x || _scaleY != y ){
            _scaleX = x;
            _scaleY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getScale(float &x, float &y){
        x = _scaleX; y = _scaleY;
    }
    
    void Sprite::setSize(float w, float h){
        setScale(w/_w, h/_h);
    }
    
    void Sprite::getSize(float &w, float &h){
        w = _w*_scaleX;
        h = _h*_scaleY;
    }
    
    void Sprite::setColor(const Color& color){
        _color = color;
    }
    
    void Sprite::setBlendMode(BlendMode blendMode){
        _blendMode = blendMode;
    }
    
    void Sprite::setZ(float z) {
        _z = z;
    }
    
    void Sprite::draw(){
        if ( _needUpdate ){
            update();
        }
        //1
		SpriteVertex v[6];
		v[0].x = _vertexPos[0].x;
		v[0].y = _vertexPos[0].y;
		v[0].z = _z;
		v[0].u = _u1;
		v[0].v = _v1;
        
		//2
		v[1].x = _vertexPos[1].x;
		v[1].y = _vertexPos[1].y;
		v[1].z = _z;
		v[1].u = _u1;
		v[1].v = _v2;
        
		//3
		v[2].x = _vertexPos[2].x;
		v[2].y = _vertexPos[2].y;
		v[2].z = _z;
		v[2].u = _u2;
		v[2].v = _v1;
		
        //3
		v[3].x = _vertexPos[2].x;
		v[3].y = _vertexPos[2].y;
		v[3].z = _z;
		v[3].u = _u2;
		v[3].v = _v1;
		
		//2
		v[4].x = _vertexPos[1].x;
		v[4].y = _vertexPos[1].y;
		v[4].z = _z;
		v[4].u = _u1;
		v[4].v = _v2;
	    
		//4
		v[5].x = _vertexPos[3].x;
		v[5].y = _vertexPos[3].y;
		v[5].z = _z;
		v[5].u = _u2;
		v[5].v = _v2;
        
        _pvb->collectVetices(v, 6, _color, _blendMode, _pTextureRes->getGlId());
    }
    
    void Sprite::update(){
        _needUpdate = false;
        PVRTMat4 m, m1;
        m = PVRTMat4::Identity();
        
        if (_posX or _posY) {
            m1 = PVRTMat4::Translation(_posX, _posY, 0.f);
            m = m * m1;
        }
		if ( _rotate != 0.f ){
			m1 = PVRTMat4::RotationZ(_rotate);
			m = m * m1;
		}
		if ( _scaleX != 1.f || _scaleY != 1.f ){
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
    
    GLuint Sprite::getGlId(){
        return _pTextureRes->getGlId();
    }

    
    //===============================================
    
    void Sprite::init(){
        _pvb = new SpriteVertexBuffer();
        
        PVRTVec2 screenSize = getScreenSize();
        _camera.lookat(PVRTVec3(0.f, 0.f, 100.f), PVRTVec3(0.f, 0.f, 0.f), PVRTVec3(0.f, 1.f, 0.f));
        _camera.ortho(screenSize.x, screenSize.y, 0, 200.f);
    }
    
    void Sprite::quit(){
        if ( _pvb ){
            delete _pvb;
            _pvb = NULL;
        }
    }
    
    void Sprite::flush(){
        _pvb->flush();
    }
    
    void Sprite::collectVetices(SpriteVertex *vertices, int numVertices, Color &color, BlendMode blendMode, GLuint textureId){
        _pvb->collectVetices(vertices, numVertices, color, blendMode, textureId);
    }
    
    void Sprite::addAtlas(const char *file){
        tinyxml2::XMLDocument doc;
        doc.LoadFile(_f(file));
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
    }
    
} //namespace lw
