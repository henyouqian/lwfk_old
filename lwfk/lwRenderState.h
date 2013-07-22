#ifndef __LW_RENDERSTATE_H__
#define __LW_RENDERSTATE_H__

namespace lw {
    
    void rsEnableBlend();
    void rsBlendFunc(GLenum sfactor, GLenum dfactor);
    
    bool rsDirty();
    void rsFlush();
    
} //namespace lw


#endif //__LW_RENDERSTATE_H__