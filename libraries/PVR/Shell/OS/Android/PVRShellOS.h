/*!***************************************************************************
 @File          Android/PVRShellOS.h

 @Title         Android/PVRShellOS

 @Brief         Initialization for the shell using Khronos EGL

 @Date          10/02/2004

 @Copyright    Copyright (C) 2005 - 2006 by Imagination Technologies Limited.

 @Platform      Non-windowed support for any Linux

 @Description   Makes programming for 3D APIs easier by wrapping surface initialization,
 				Texture allocation and other functions for use by a demo.
*****************************************************************************/
#ifndef _PVRSHELLOS_
#define _PVRSHELLOS_

#include <android_native_app_glue.h>
#include <time.h>

#define PVRSHELL_DIR_SYM	'/'
#define _stricmp strcasecmp

/*!***************************************************************************
 PVRShellInitOS
 @Brief Class. Interface with specific Operative System.
*****************************************************************************/
class PVRShellInitOS
{
public:
	~PVRShellInitOS()
	{
	}

public:
	timeval 	 m_StartTime;

	android_app* m_pAndroidState;
	bool m_bRendering;
	bool m_bError;
};

#endif /* _PVRSHELLOS_ */
/*****************************************************************************
 End of file (PVRShellOS.h)
*****************************************************************************/
