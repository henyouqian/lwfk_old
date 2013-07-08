#include "lwPrefix.h"
#include "lwApp.h"
#include "lwTask.h"
#include "lwSprite.h"
#include "PVRShell.h"
#include "PVRTResourceFile.h"
#include "lwLog.h"

namespace {
    class App : public PVRShell
    {
    public:
        App(){}
        virtual bool InitApplication() {
            CPVRTResourceFile::SetReadPath((char*)PVRShellGet(prefReadPath));
            CPVRTResourceFile::SetLoadReleaseFunctions(PVRShellGet(prefLoadFileFunc), PVRShellGet(prefReleaseFileFunc));
            return true;
        }
        virtual bool InitView() {
            lw::Sprite::init();
            lw::Task::init();
            lwapp_init();
            return true;
        }
        virtual bool ReleaseView() {
            lwapp_quit();
            lw::Task::quit();
            lw::Sprite::quit();
            return true;
        }
        virtual bool QuitApplication() {
            return true;
        }
        virtual bool RenderScene() {
            lw::Task::update();
            lw::Task::draw();
            lw::Sprite::flush();
            return true;
        }
    };
    
    App* _app = NULL;
}


PVRShell* NewDemo()
{
    _app = new App();
	return _app;
}

PVRTVec2 getScreenSize() {
    float width = (float)_app->PVRShellGet(prefWidth);
    float height = (float)_app->PVRShellGet(prefHeight);
    return PVRTVec2(width, height);
}

const char* getReadPath() {
    return (const char*)_app->PVRShellGet(prefReadPath);
}
