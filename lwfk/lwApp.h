#ifndef __LW_APP_H__
#define __LW_APP_H__

void lwapp_init();
void lwapp_quit();

namespace lw {
    PVRTVec2 screenSize();
    const char* readPath();
}


#endif //__LW_APP_H__