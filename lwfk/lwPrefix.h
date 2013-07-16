//
// Prefix header for all source files of the 'lwfk' target in the 'lwfk' project
//

#ifdef __OBJC__
    #import <Foundation/Foundation.h>
#endif

#ifdef __cplusplus
    #include <vector>
    #include <list>
    #include <string>
    #include <cassert>
    #include <iostream>
    #include <map>
    #include <limits>
    #include "PVRTMatrix.h"
    #include "PVRTFixedPoint.h"
    #include "PVRTVector.h"
    #include "PVRTTrans.h"

    #ifdef __APPLE__
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #else
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
    #endif
#endif