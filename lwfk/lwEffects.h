#ifndef __LW_EFFECTS_H__
#define __LW_EFFECTS_H__

#include "lwfk/lwRes.h"

namespace lw
{
    
    
class EffectsRes : public Res
{
public:
    enum Semantic
    {
        UNKNOWN,
        POSITION,
        NORMAL,
        BINORMAL,
        TANGENT,
        UV0,
        UV1,
        UV2,
        UV3,
        WORLDVIEW,
        WORLDVIEWPROJ,
        WORLDVIEWIT,
    };
    
    /*
     _position,
     _normal,
     _binormal,
     _tangent,
     _uv0,
     _uv1,
     _uv2,
     _uv3,
     _mv,
     _mvp,
     _mvi,
     
     */
    
    struct LocSmt
    {
        int location;
        Semantic semantic;
    };
    
    static EffectsRes* create(const char *file);
    GLuint getProgram();
    int getLocationFromSemantic(EffectsRes::Semantic semantic);
    int getUniformLocation(const char* name);
    void use();
    
    const std::vector<EffectsRes::LocSmt>& getLocSmts();
    
private:
    EffectsRes(const char *file, bool &ok);
    ~EffectsRes();
    
    GLuint _program;
    std::vector<LocSmt> _locSmts;
};
    
    
}//namespace lw

#endif //__LW_EFFECTS_H__