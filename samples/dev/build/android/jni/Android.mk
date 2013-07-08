MY_DIR := $(call my-dir)
PVRSDKDIR := $(MY_DIR)/../../../../../libraries/PVR

include $(PVRSDKDIR)/Tools/OGLES2/Build/Android/Android.mk
include $(MY_DIR)/OGLES2IntroducingPFX/Android.mk
