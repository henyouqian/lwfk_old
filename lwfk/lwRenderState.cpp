#include "lwPrefix.h"
#include "lwRenderState.h"
#include "lwLog.h"

namespace lw {

    
    namespace {
        
        class Cmd {
        public:
            Cmd(unsigned char type): _type(type) {}
            virtual ~Cmd() {}
            virtual void set() = 0;
            
            void addTo(std::vector<Cmd*>* cmds) {
                std::vector<Cmd*>::iterator it = cmds->begin();
                std::vector<Cmd*>::iterator itend = cmds->end();
                for (; it != itend; ++it) {
                    if ((*it)->_type == _type) {
                        delete *it;
                        cmds->erase(it);
                        break;
                    }
                }
                cmds->push_back(this);
            }
        protected:
            unsigned char _type;
        };
        
        
        std::vector<Cmd*> _cmdSets[2];
        std::vector<Cmd*>* _setCmds = &_cmdSets[0];
        std::vector<Cmd*>* _resetCmds = &_cmdSets[1];
        
        class Cleaner{
        public:
            Cleaner () {
                
            }
            ~Cleaner() {
                std::vector<Cmd*>::iterator it = _setCmds->begin();
                std::vector<Cmd*>::iterator itend = _setCmds->end();
                for (; it != itend; ++it) {
                    delete (*it);
                }
                it = _resetCmds->begin();
                itend = _resetCmds->end();
                for (; it != itend; ++it) {
                    delete (*it);
                }
            }
        };
        Cleaner _cleaner;
        
        enum {
            BLEND,
            BLEND_FUNC,
            DEPTH_TEST,
            DEPTH_MASK,
            CULL_FACE,
        };
        
    }
    
    
    //===================================================
    class CmdBlend :public Cmd {
    public:
        CmdBlend(bool b):Cmd(BLEND), _enable(b) {}
        
        virtual void set() {
            if (_enable) {
                glEnable(GL_BLEND);
            } else {
                glDisable(GL_BLEND);
            }
        }
    private:
        bool _enable;
    };

    void rsBlend(bool enable) {
        if (!enable)
            return;
        CmdBlend *pCmd = new CmdBlend(true);
        pCmd->addTo(_setCmds);
        CmdBlend *pCmdReset = new CmdBlend(false);
        pCmdReset->addTo(_resetCmds);
    }
    
    
    //===================================================
    class CmdBlendFunc :public Cmd {
    public:
        CmdBlendFunc(GLenum sfactor, GLenum dfactor)
        :Cmd(BLEND_FUNC), _sfactor(sfactor), _dfactor(dfactor) {}
        
        virtual void set() {
            glBlendFunc(_sfactor, _dfactor);
        }
    private:
        GLenum _sfactor, _dfactor;
    };
    
    void rsBlendFunc(GLenum sfactor, GLenum dfactor) {
        glBlendFunc(sfactor, dfactor);
//        if (sfactor == GL_SRC_ALPHA && dfactor == GL_ONE_MINUS_SRC_ALPHA) {
//            return;
//        }
//        CmdBlendFunc *pCmd = new CmdBlendFunc(sfactor, dfactor);
//        pCmd->addTo(_setCmds);
//        CmdBlendFunc *pCmdReset = new CmdBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        pCmdReset->addTo(_resetCmds);
    }
    
    
    //===================================================
    class CmdDepthTest :public Cmd {
    public:
        CmdDepthTest(bool b):Cmd(DEPTH_TEST), _enable(b) {}
        
        virtual void set() {
            if (_enable) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
        }
    private:
        bool _enable;
    };
    
    void rsDepthTest(bool enable) {
        if (!enable)
            return;
        CmdDepthTest *pCmd = new CmdDepthTest(true);
        pCmd->addTo(_setCmds);
        CmdDepthTest *pCmdReset = new CmdDepthTest(false);
        pCmdReset->addTo(_resetCmds);
    }
    
    
    //===================================================
    class CmdDepthMask :public Cmd {
    public:
        CmdDepthMask(bool b):Cmd(DEPTH_MASK), _enable(b) {}
        
        virtual void set() {
            if (_enable) {
                glDepthMask(GL_TRUE);
            } else {
                glDepthMask(GL_FALSE);
            }
        }
    private:
        bool _enable;
    };
    
    void rsDepthMask(bool enable) {
        if (enable)
            return;
        CmdDepthMask *pCmd = new CmdDepthMask(false);
        pCmd->addTo(_setCmds);
        CmdDepthMask *pCmdReset = new CmdDepthMask(true);
        pCmdReset->addTo(_resetCmds);
    }
    
    
    //===================================================
    class CmdCullFace :public Cmd {
    public:
        CmdCullFace(bool b):Cmd(CULL_FACE), _enable(b) {}
        
        virtual void set() {
            if (_enable) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }
        }
    private:
        bool _enable;
    };
    
    void rsCullFace(bool enable) {
        if (!enable)
            return;
        CmdCullFace *pCmd = new CmdCullFace(true);
        pCmd->addTo(_setCmds);
        CmdCullFace *pCmdReset = new CmdCullFace(false);
        pCmdReset->addTo(_resetCmds);
    }
    
    
    //===================================================
    void rsFlush() {
        std::vector<Cmd*>::iterator it = _setCmds->begin();
        std::vector<Cmd*>::iterator itend = _setCmds->end();
        for (; it != itend; ++it) {
            (*it)->set();
        }
        _setCmds->clear();
        std::vector<Cmd*>* other = _setCmds;
        _setCmds = _resetCmds;
        _resetCmds = other;
    }
    
    void rsInit() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    //======================================
    //RsObjBlend
    RsObjBlend::RsObjBlend(bool enable)
    :_enable(enable) {
    }
    
    void RsObjBlend::use() {
        rsBlend(_enable);
    }

    //RsObjBlendFunc
    RsObjBlendFunc::RsObjBlendFunc(GLenum sfactor, GLenum dfactor)
    :_sfactor(sfactor), _dfactor(dfactor){
    }
    
    void RsObjBlendFunc::use() {
        rsBlendFunc(_sfactor, _dfactor);
    }
    
    //RsObjDepthTest
    RsObjDepthTest::RsObjDepthTest(bool enable)
    :_enable(enable) {
    }
    
    void RsObjDepthTest::use() {
        rsDepthTest(_enable);
    }
    
    //RsObjDepthMask
    RsObjDepthMask::RsObjDepthMask(bool enable)
    :_enable(enable) {
    }
    
    void RsObjDepthMask::use() {
        rsDepthMask(_enable);
    }
    
    //RsObjCullFace
    RsObjCullFace::RsObjCullFace(bool enable)
    :_enable(enable) {
    }
    
    void RsObjCullFace::use() {
        rsCullFace(_enable);
    }
}
