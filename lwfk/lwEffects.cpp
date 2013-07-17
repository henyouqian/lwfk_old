#include "lwPrefix.h"
#include "lwEffects.h"
#include "lwFileSys.h"
#include "libraries/tinyxml2/tinyxml2.h"
#include "lwLog.h"
#include "PVRTResourceFile.h"

namespace lw
{

    namespace
    {
        
        ResMgr _resMgr;

        bool compileShader(GLuint &shader, GLenum type, const GLchar *source)
        {
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
        
    } //namespace
    
    //==========================================
    class EffectPass {
    public:
        EffectPass(const tinyxml2::XMLElement *pElemPass, std::vector<NameShader>& vtxShaders, std::vector<NameShader>& fragShaders);
        ~EffectPass();
        
    public:
        GLuint program;
    };
    
    EffectPass::EffectPass(const tinyxml2::XMLElement *pElemPass, std::vector<NameShader>& vtxShaders, std::vector<NameShader>& fragShaders) {
        program = 0;
        
        const char *vs = pElemPass->Attribute("vs");
        const char *fs = pElemPass->Attribute("fs");
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
    }
    
    EffectPass::~EffectPass() {
        if (program)
            glDeleteProgram(program);
    }
    
    //==========================================
    class EffectFx {
    public:
        EffectFx(const tinyxml2::XMLElement *pElemFx, std::vector<NameShader>& vtxShaders, std::vector<NameShader>& fragShaders);
        ~EffectFx();
        
    public:
        std::string name;
        std::vector<EffectPass*> passes;
        bool ok;
    };
    
    EffectFx::EffectFx(const tinyxml2::XMLElement *pElemFx, std::vector<NameShader>& vtxShaders, std::vector<NameShader>& fragShaders) {
        ok = false;
        name = pElemFx->Attribute("name");
        
        const tinyxml2::XMLElement *pElemPass = pElemFx->FirstChildElement("pass");
        while (pElemPass) {
            EffectPass *pPass = new EffectPass(pElemPass, vtxShaders, fragShaders);
            if (pPass->program)
                passes.push_back(pPass);
            else {
                delete pPass;
                lwerror("create pass failed: %s", name.c_str());
                return;
            }
            pElemPass = pElemPass->NextSiblingElement("pass");
        }
        ok = true;
    }
    
    EffectFx::~EffectFx() {
        std::vector<EffectPass*>::iterator it = passes.begin();
        std::vector<EffectPass*>::iterator itend = passes.end();
        for (; it != itend; ++it) {
            delete (*it);
        }
    }
    
    
    
    //==========================================
    EffectsRes* EffectsRes::create(const char *file)
    {
        assert(file);
        EffectsRes *pRes = (EffectsRes*)_resMgr.getRes(file);
        if (pRes)
            return pRes;
        
        bool ok = false;
        pRes = new EffectsRes(file, ok);
        if (pRes && !ok) {
            lwerror("new EffectsRes failed");
            delete pRes;
            return NULL;
        }
        return pRes;
    }

    EffectsRes::EffectsRes(const char *file, bool &ok)
        :Res(file, _resMgr)
    {
        ok = false;
        assert(file);
        
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

    EffectsRes::~EffectsRes()
    {
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (; it != itend; ++it) {
            delete (*it);
        }
    }
    
    GLuint EffectsRes::getProgram(const char *fxName, int pass) {
        assert(fxName && pass >= 0);
        
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (;it != itend; ++it) {
            if ((*it)->name.compare(fxName) == 0) {
                if (pass < (*it)->passes.size())
                    return ((*it)->passes)[pass]->program;
            }
        }
        return 0;
    }

    int EffectsRes::getLocationFromSemantic(Semantic semantic)
    {
        std::vector<LocSmt>::iterator it = _locSmts.begin();
        std::vector<LocSmt>::iterator itend = _locSmts.end();
        for (; it != itend; ++it) {
            if (it->semantic == semantic) {
                return it->location;
            }
        }
        lwerror("sementic not found: %d", semantic);
        return -1;
    }

    int EffectsRes::getUniformLocation(const char* uniform, const char *fxName, int pass)
    {
        GLuint program = getProgram(fxName, pass);
        return glGetUniformLocation(program, uniform);
    }

    const std::vector<EffectsRes::LocSmt>& EffectsRes::getLocSmts()
    {
        return _locSmts;
    }
    
    int EffectsRes::getPassCount(const char *fxName) {
        assert(fxName);
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (;it != itend; ++it) {
            if ((*it)->name.compare(fxName) == 0)
                return (*it)->passes.size();
        }
        return 0;
    }

    void EffectsRes::use(const char* fxName, int pass)
    {
        assert(fxName && pass >= 0);
        
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (;it != itend; ++it) {
            if ((*it)->name.compare(fxName) == 0) {
                if (pass < (*it)->passes.size()) {
                    glUseProgram(((*it)->passes)[pass]->program);
                }
                break;
            }
        }
    }
    
    bool EffectsRes::checkFxName(const char *fxName) {
        assert(fxName);
        
        std::vector<EffectFx*>::iterator it = _fxs.begin();
        std::vector<EffectFx*>::iterator itend = _fxs.end();
        for (;it != itend; ++it) {
            if ((*it)->name.compare(fxName) == 0) {
                return true;
            }
        }
        return false;
    }
    
} //namespace lw
