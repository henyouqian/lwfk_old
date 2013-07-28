#ifndef __LW_RENDERSTATE_H__
#define __LW_RENDERSTATE_H__

namespace lw {
    
    void rsBlend(bool enable);   //default: false
    void rsBlendFunc(GLenum src, GLenum dst);   //default: sfactor=GL_SRC_ALPHA, dfactor=GL_ONE_MINUS_SRC_ALPHA
    void rsDepthTest(bool enable);  //default: false
    void rsDepthMask(bool enable);  //default: false
    void rsCullFace(bool enable);   //default: false
    
    
    void rsFlush();
    
    class RsObj {
    public:
        virtual ~RsObj() {}
        virtual void use() = 0;
    };
    
    //===================================
    //RsObjBlend
    class RsObjBlend : public RsObj {
    public:
        RsObjBlend(bool enable);
        virtual void use();
    private:
        bool _enable;
    };
    
    //RsObjBlendFunc
    class RsObjBlendFunc : public RsObj {
    public:
        RsObjBlendFunc(GLenum sfactor, GLenum dfactor);
        virtual void use();
    private:
        GLenum _sfactor, _dfactor;
    };
    
    //RsObjDepthTest
    class RsObjDepthTest : public RsObj {
    public:
        RsObjDepthTest(bool enable);
        virtual void use();
    private:
        bool _enable;
    };
    
    //RsObjDepthMask
    class RsObjDepthMask : public RsObj {
    public:
        RsObjDepthMask(bool enable);
        virtual void use();
    private:
        bool _enable;
    };
    
    //RsObjCullFace
    class RsObjCullFace : public RsObj {
    public:
        RsObjCullFace(bool enable);
        virtual void use();
    private:
        bool _enable;
    };
    
} //namespace lw


#endif //__LW_RENDERSTATE_H__