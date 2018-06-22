#MAKE FILE FOR CLCommon
PROJECT_NAME=CLCommon
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETLIB = $(OUTPUT_LIB_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).a

INCLUDES = \
	-I. \
	-I../Common \
	-I$(COMMON_PROJ_DIR) \
	-I$(APPSDK_DIR)/include \
	-I$(CAL_DIR)/include \
	-I$(DYNAMICLIBRARYMODULE_DIR) \
	-I$(DEVICEINFO_DIR) \
	-I$(TSINGLETON_DIR)

LIB_OBJS =  \
	./$(OBJ_DIR)/CLInternalFunctionDefs.o \
	./$(OBJ_DIR)/CLFunctionDefs.o \
	./$(OBJ_DIR)/CLFunctionEnumDefs.o \
	./$(OBJ_DIR)/CLUtils.o \
	./$(OBJ_DIR)/CLCUInfoBase.o \
	./$(OBJ_DIR)/CLPlatformInfo.o \
	./$(OBJ_DIR)/OpenCLModule.o \
	./$(OBJ_DIR)/CLDeviceReplacer.o \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
