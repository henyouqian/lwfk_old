//
// Prefix header for all source files of the 'dev' target in the 'dev' project
//

#ifdef __IPHONE_5_0
    #import <Availability.h>
    #import <TargetConditionals.h>
#endif

#ifdef __OBJC__
    #import <UIKit/UIKit.h>
    #import <Foundation/Foundation.h>
#endif

#ifdef __cplusplus
    #include "lwfk/lwPrefix.h"
    #include <unistd.h>
#endif

