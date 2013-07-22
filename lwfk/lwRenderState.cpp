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
                lw::rsBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
            ENABLE_BLEND,
            BLEND_FUNC,
        };
        
    }
    
    
    
    class CmdEnableBlend :public Cmd {
    public:
        CmdEnableBlend(bool b):Cmd(ENABLE_BLEND), _enable(b) {}
        
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


    void rsEnableBlend() {
        CmdEnableBlend *pCmd = new CmdEnableBlend(true);
        pCmd->addTo(_setCmds);
        CmdEnableBlend *pCmdReset = new CmdEnableBlend(false);
        pCmdReset->addTo(_resetCmds);
    }
    
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
        if (sfactor == GL_SRC_ALPHA || dfactor == GL_ONE_MINUS_SRC_ALPHA) {
            return;
        }
        CmdBlendFunc *pCmd = new CmdBlendFunc(sfactor, dfactor);
        pCmd->addTo(_setCmds);
        CmdBlendFunc *pCmdReset = new CmdBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        pCmdReset->addTo(_resetCmds);
    }
    
    bool rsDirty() {
        return !_setCmds->empty();
    }
    
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

}

