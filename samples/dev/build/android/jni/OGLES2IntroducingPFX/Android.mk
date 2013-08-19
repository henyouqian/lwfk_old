LOCAL_PATH := $(realpath $(call my-dir)/../../../../../..)
PVRSDKDIR := $(LOCAL_PATH)/libraries/PVR

ASSETDIR := $(realpath $(call my-dir)/../assets)
PROJDIR := $(LOCAL_PATH)/samples/dev
ANDROID_PROJ_DIR := $(PROJDIR)/build/android

include $(CLEAR_VARS)

LOCAL_MODULE    := OGLES2IntroducingPFX

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := \
					libraries/PVR/Shell/PVRShell.cpp \
					libraries/PVR/Shell/API/KEGL/PVRShellAPI.cpp \
				  	libraries/PVR/Shell/OS/Android/PVRShellOS.cpp \
				   	lwfk/lwApp.cpp \
					lwfk/lwCamera.cpp \
					lwfk/lwColor.cpp \
					lwfk/lwEffects.cpp \
					lwfk/lwlog.cpp \
					lwfk/lwMaterial.cpp \
					lwfk/lwMesh.cpp \
					lwfk/lwModel.cpp \
					lwfk/lwRes.cpp \
					lwfk/lwSprite.cpp \
					lwfk/lwRenderState.cpp \
					lwfk/lwTask.cpp \
					lwfk/lwTexture.cpp \
					lwfk/lwTouch.cpp \
					
					libraries/soil/SOIL.c \
				   	libraries/soil/image_DXT.c \
				   	libraries/soil/image_helper.c \
				   	libraries/soil/stb_image_aug.c \
				   	libraries/tinyxml2/tinyxml2.cpp \
				   	samples/dev/app.cpp \
					samples/dev/task/sliderTask.cpp \
					samples/dev/task/spriteTask.cpp \

LOCAL_C_INCLUDES :=	\
				    $(PVRSDKDIR)/Shell	\
				    $(PVRSDKDIR)/Shell/API/KEGL	\
				    $(PVRSDKDIR)/Shell/OS/Android	\
				    $(PVRSDKDIR)/Builds/Include	\
				    $(PVRSDKDIR)/Tools	\
				    $(PVRSDKDIR)/Tools/OGLES2 \
				    sources/cxx-stl/stlport/stlport \
				    $(LOCAL_PATH)/libraries \
				    $(LOCAL_PATH)/libraries/soil \
				    $(PROJDIR) \
				    $(PROJDIR)/task

LOCAL_CFLAGS := -DBUILD_OGLES2


LOCAL_LDLIBS :=  \
				-llog \
				-landroid \
				-lEGL \
				-lGLESv2

LOCAL_STATIC_LIBRARIES := \
				          android_native_app_glue \
				          ogles2tools 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)



$(ASSETDIR):
	-mkdir $(ASSETDIR)




