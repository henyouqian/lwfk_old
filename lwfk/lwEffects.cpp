#include "lwPrefix.h"
#include "lwEffects.h"
#include "lwFileSys.h"
#include "libraries/tinyxml2/tinyxml2.h"
#include "lwLog.h"
#include "PVRTResourceFile.h"
#include "lwRenderState.h"

namespace lw {

    namespace {
        
        KeyResMgr _resMgr;

        bool compileShader(GLuint &shader, GLenum type, const GLchar *source) {
            shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, NULL);
            glCompileShader(shader);
            
    #if defined(DEBUG)
            GLint logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                GLchar *log = (GLchar *)malloc(logLength);
                glGetShaderInfoLog(shader, logLength, &logLength, log);
                lwerror("compile shader log: %s", log);
                free(log);
            }
    #endif
            
            GLint status;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if (status == 0) {
                glDeleteShader(shader);
                return false;
            }
            
            return true;
        }
        
        bool linkProgram(GLuint prog)
        {
            GLint status;
            glLinkProgram(prog);
            glGetProgramiv(prog, GL_LINK_STATUS, &status);
            if (status == 0) {
                return false;
            }
            
            return true;
        }
        
        struct NameShader {
            std::string name;
            GLuint shader;
        };
        
        std::map<std::string, GLenum> _blendFactorMap;
        
        GLenum getBlendFactor(const char *str) {
            assert(str);
            if ( _blendFactorMap.empty()) {
                _blendFactorMap["GL_ZERO"] = GL_ZERO;
                _blendFactorMap["GL_ONE"] = GL_ONE;
                _blendFactorMap["GL_SRC_COLOR"] = GL_SRC_COLOR;
                _blendFactorMap["GL_ONE_MINUS_SRC_COLOR"] = GL_ONE_MINUS_SRC_COLOR;
                _blendFactorMap["GL_DST_COLOR"] = GL_DST_COLOR;
                _blendFactorMap["GL_ONE_MINUS_DST_COLOR"] = GL_ONE_MINUS_DST_COLOR;
                _blendFactorMap["GL_SRC_ALPHA"] = GL_SRC_ALPHA;
                _blendFactorMap["GL_ONE_MINUS_SRC_ALPHA"] = GL_ONE_MINUS_SRC_ALPHA;
                _blendFactorMap["GL_DST_ALPHA"] = GL_DST_ALPHA;
                _blendFactorMap["GL_ONE_MINUS_DST_ALPHA"] = GL_ONE_MINUS_DST_ALPHA;
                _blendFactorMap["GL_CONSTANT_COLOR"] = GL_CONSTANT_COLOR;
                _blendFactorMap["GL_ONE_MINUS_CONSTANT_COLOR"] = GL_ONE_MINUS_CONSTANT_COLOR;
                _blendFactorMap["GL_CONSTANT_ALPHA"] = GL_CONSTANT_ALPHA;
                _blendFactorMap["GL_ONE_MINUS_CONSTANT_ALPHA"] = GL_ONE_MINUS_CONSTANT_ALPHA;
                _blendFactorMap["GL_SRC_ALPHA_SATURATE"] = GL_SRC_ALPHA_SATURATE;
            }
            std::map<std::string, GLenum>::iterator it = _blendFactorMap.find(str);
            if (it == _blendFactorMap.end()) {
                lwerror("blend factor not found: %s", str);
                return GL_ZERO;
            }
            return it->second;
        }
        
    } //namespace
    
    //==========================================
    class EffectFx {
    public:
        EffectFx(const tinyxml2::XMLElement *pElemFx, std::vector<NameShader>& vtxShaders, std::vector<NameShader>& fragShaders);
        ~EffectFx();
        
    public:
        std::string name;
        GLuint program;
        std::vector<RsObj*> rsObjs;
        bool ok;
    };
    
    EffectFx::EffectFx(const tinyxml2::XMLElement *pElemFx, std::vector<NameShader>& vtxShaders, std::vector<NameShader>& fragShaders) {
        ok = false;
        name = pElemFx->Attribute("name");
        
        const char *vs = pElemFx->Attribute("vs");
        const char *fs = pElemFx->Attribute("fs");
        assert(vs && fs);
        
        NameShader *pVtxNameShader = NULL;
        NameShader *pFragNameShader = NULL;
        std::vector<NameShader>::iterator it = vtxShaders.begin();
        std::vector<NameShader>::iterator itend = vtxShaders.end();
        for (; it != itend; ++it) {
            if (it->name.compare(vs) == 0) {
                pVtxNameShader = &(*it);
                break;
            }
        }
        
        it = fragShaders.begin();
        itend = fragShaders.end();
        for (; it != itend; ++it) {
            if (it->name.compare(fs) == 0) {
                pFragNameShader = &(*it);
                break;
            }
        }
        
        assert(pVtxNameShader && pFragNameShader);
        
        program = glCreateProgram();
        glAttachShader(program, pVtxNameShader->shader);
        glAttachShader(program, pFragNameShader->shader);
        
        if (!linkProgram(program)) {
            lwerror("Failed to link program: %u", program);
            assert(0);
            return;
        }
        
        glDetachShader(program, pVtxNameShader->shader);
        glDetachShader(program, pFragNameShader->shader);
        
        //render states
        const tinyxml2::XMLElement *pElemRS = pElemFx->FirstChildElement();
        while (pElemRS) {
            if (strcmp(pElemRS->Name(), "rsBlend") == 0) {
                bool enable = false;
                if (pElemRS->QueryAttribute("enable", &enable)) {
                    lwerror("<rsBlend enable> attribute error");
                } else {
                    RsObjBlend *rsobj = new RsObjBlend(enable);
                    rsObjs.push_back(rsobj);
                }
            } else if (strcmp(pElemRS->Name(), "rsBlendFunc") == 0) {
                const char *src = pElemRS->Attribute("src");
                const char *dst = pElemRS->Attribute("dst");
                if (src && dst) {
                    RsObjBlendFunc *rsobj = new RsObjBlendFunc(getBlendFactor(src), getBlendFactor(dst));
                    rsObjs.push_back(rsobj);
                } else {
                    lwerror("<rsBlendFunc src, dst> attribute error");
                }
            } else if (strcmp(pElemRS->Name(), "rsDepthTest") == 0) {
                bool enable = false;
                if (pElemRS->QueryAttribute("enable", &enable)) {
                    lwerror("<rsDepthTest enable> attribute error");
                } else {
                    RsObjDepthTest *rsobj = new RsObjDepthTest(enable);
                    rsObjs.push_back(rsobj);
                }
            } else if (strcmp(pElemRS->Name(), "rsDepthMask") == 0) {
                bool enable = false;
                if (pElemRS->QueryAttribute("enable", &enable)) {
                    lwerror("<rsDepthMask enable> attribute error");
                } else {
                    RsObjDepthMask *rsobj = new RsObjDepthMask(enable);
                    rsObjs.push_back(rsobj);
                }
            } else if (strcmp(pElemRS->Name(), "rsCullFace") == 0) {
                bool enable = false;
                if (pElemRS->QueryAttribute("enable", &enable)) {
                    lwerror("<rsCullFace enable> attribute error");
                } else {
                    RsObjCullFace *rsobj = new RsObjCullFace(enable);
                    rsObjs.push_back(rsobj);
                }
            }
            pElemRS = pElemRS->NextSiblingElement();
        }
        
        ok = true;
    }
    
    EffectFx::~EffectFx() {
        if (program)
            glDeleteProgram(program);
        
        std::vector<RsObj*>::iterator it = rsObjs.begin();
        std::vector<RsObj*>::iterator itend = rsObjs.end();
        for (; it != itend; ++it) {
            delete (*it);
        }
    }
    
    
    
    //==========================================
    EffectsRes* EffectsRes::create(const char *file) {
        assert(file);
        EffectsRes *pRes = (EffectsRes*)_resMgr.get(file);
        if (pRes)
            return pRes;
        
        bool ok = false;
        pRes = new EffectsRes(file, ok);
        if (pRes && !ok) {
            lwerror("new EffectsRes failed");
            delete pRes;
            return NULL;
        }
        _resMgr.add(file, pRes);
        return pRes;
    }

    EffectsRes::EffectsRes(const char *file, bool &ok) {
        assert(file);
        ok = false;
        _fileName = file;
        
        std::vector<NameShader> _vtxShaders;
        std::vector<NameShader> _fragShaders;
        
        tinyxml2::XMLDocument doc;
        CPVRTResourceFile resFile(file);
        tinyxml2::XMLError err = doc.Parse((const char*)resFile.DataPtr(), resFile.Size());
        assert(err == tinyxml2::XML_SUCCESS);
        
        //create vertex shaders
        const tinyxml2::XMLElement *pElemLwfx = doc.RootElement();
        const tinyxml2::XMLElement *pElemVS = pElemLwfx->FirstChildElement("vs");
        while (pElemVS) {
            const char *vsSource = pElemVS->GetText();
            const char *vsName = pElemVS->Attribute("name");
            assert(vsSource && vsName);
            GLuint vtxShader;
            if (!compileShader(vtxShader, GL_VERTEX_SHADER, vsSource)) {
                return;
            }
            NameShader ns = {vsName, vtxShader};
            _vtxShaders.push_back(ns);
            
            pElemVS = pElemVS->NextSiblingElement("vs");
        }
        
        //create fragment shaders
        const tinyxml2::XMLElement *pElemFS = pElemLwfx->FirstChildElement("fs");
        while (pElemFS) {
            const char *fsSource = pElemFS->GetText();
            const char *fsName = pElemFS->Attribute("name");
            assert(fsSource && fsName);
            GLuint fragShader;
            if (!compileShader(fragShader, GL_FRAGMENT_SHADER, fsSource)) {
                return;
            }
            NameShader ns = {fsName, fragShader};
            _fragShaders.push_back(ns);
            
            pElemFS = pElemFS->NextSiblingElement("vs");
        }
        
        //create programs
        const tinyxml2::XMLElement *pElemFx = pElemLwfx->FirstChildElement("fx");
        while (pElemFx) {
            EffectFx *pfx = new EffectFx(pElemFx, _vtxShaders, _fragShaders);
            if (!pfx->ok) {
                delete pfx;
                lwerror("create EffectFx failed");
                break;
            }
            _fxs.push_back(pfx);
            
            pElemFx = pElemFx->NextSiblingElement("fx");
        }
        
        //delete shaders
        std::vector<NameShader>::iterator it = _vtxShaders.begin();
        std::vector<NameShader>::iterator itend = _vtxShaders.end();
        for (;it != itend; ++it) {
            glDeleteShader(it->shader);
        }
        
        it = _fragShaders.begin();
        itend = _fragShaders.end();
        for (;it != itend; ++it) {
            glDeleteShader(it->shader);
        }
        
                
        ok = true;
    }

    EffectsRes::~EffectsRes() {
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (; it != itend; ++it) {
            delete (*it);
        }
        _resMgr.del(_fileName.c_str());
    }
    
    GLuint EffectsRes::getProgram(const char *fxName) {
        assert(fxName);
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (;it != itend; ++it) {
            if ((*it)->name.compare(fxName) == 0) {
                return (*it)->program;
            }
        }
        return 0;
    }
    
    int EffectsRes::getProgramAndRenderStates(const char *fxName, GLuint &program, const std::vector<RsObj*> *&renderStates ) {
        assert(fxName);
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (;it != itend; ++it) {
            if ((*it)->name.compare(fxName) == 0) {
                program = (*it)->program;
                renderStates = &((*it)->rsObjs);
                return 0;
            }
        }
        return -1;
    }

    
} //namespace lw
