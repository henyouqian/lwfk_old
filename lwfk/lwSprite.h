#ifndef __LW_SPRITE_H__
#define __LW_SPRITE_H__

#include "lwfk/lwColor.h"

namespace lw{
    
    class Material;
    class TextureRes;
    
    struct SpriteVertex{
        float x, y, z;
        float u, v;
        float r, g, b, a;
    };
    
	class Sprite{
    public:
        static Sprite* createFromFile(const char* textureFile, const char* fxName);
        static Sprite* createFromAtlas(const char* key, const char* fxName);
        ~Sprite();
        void setUV(float u, float v, float w, float h);
        void getUV(float &u, float &v, float &w, float &h);
		void setAnchor(float x, float y);
        void getAnchor(float &x, float &y);
        void setPos(float x, float y);
		void getPos(float &x, float &y);
        void setRotate(float r);
		float getRotate();
		void setScale(float x, float y);
		void getScale(float &x, float &y);
		void setSize(float w, float h);
		void getSize(float &w, float &h);
        void setColor(const Color &color);
        void setZ(float z);
        void draw();
//        GLuint getGlId();
        
    private:
        void uvInit();
        
        Material *_pMaterial;
        TextureRes *_pTexture;
        std::string _materialKey;
        float _ancX, _ancY;
		float _posX, _posY;
		float _rotate;
		float _scaleX, _scaleY;
        Color _color;
        float _u, _v, _w, _h;
        float _u1, _v1, _u2, _v2;
        float _z;
        PVRTVec2 _vertexPos[4];
        bool _needUpdate;
        
        Sprite(const char *textureFile, const char *fxName, bool fromAtlas, bool &ok);
        int loadFromFile(const char* textureFile, const char* fxName);
        int loadFromAtlas(const char *key);
        void update();
        
    public:
        static void init();
        static void quit();
        static void flush();
        
        static void addAtlas(const char *file);
    };
    
    
    
	
} //namespace lw

#endif //__LW_SPRITE_H__