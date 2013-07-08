#include "lwPrefix.h"
#include "lwLog.h"

#ifdef ANDROID
    #include <android/log.h>

    void ainfo(const char* msg) {
        __android_log_print(ANDROID_LOG_INFO, "lw", msg);
    }
#else
    void ainfo(const char* msg) {
        std::cout<<"ainfo: " << FLE << " - " << __LINE__ << " - " << __FUNCTION__ << ":\n" << msg << "\n";
    }
#endif