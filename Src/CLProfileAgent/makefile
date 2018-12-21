#MAKE FILE FOR CLProfileAgent
PROJECT_NAME=CLProfileAgent
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETSO = $(OUTPUT_BIN_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).so

ifeq ($(SKIP_HSA), 1)
	COMGRUTIL_SRC=
else
	COMGRUTIL_SRC=./$(OBJ_DIR)/ComgrUtils.o
        COMGR_INCLUDES= \
			-I$(HSA_DIR)/include \
			-I$(HSA_DIR)/include/hsa \
			-I$(COMGR_DIR) \
			-I$(HSA_DIR)/../include
endif

INCLUDES = \
	-I. \
	-I$(SRC_COMMON_DIR) \
	-I$(SRC_CLCOMMON_DIR) \
	-I$(COMMON_PROJ_DIR) \
	-I$(GPU_PERF_API_DIR) \
	-I$(APPSDK_DIR)/include \
	-I$(CAL_DIR)/include \
	-I$(DYNAMICLIBRARYMODULE_DIR) \
	-I$(DEVICEINFO_DIR) \
	-I$(TSINGLETON_DIR) \
	-isystem$(BOOST_DIR) \
	-I$(UTF8CPP_DIR) \
	-I$(ACL_DIR)/include \
	-I$(COMGRUTILS_DIR) \
	-I$(ADL_DIR)/include \
	-I$(ADLUTIL_DIR) \
	-I$(ACLMODULEMANAGER_DIR) \
	$(COMGR_INCLUDES) \

LIBS = \
	$(CLCOMMON_LIBS) \
	$(COMMON_LIBS) \
	$(FRAMEWORK_LIBS) \
	$(STANDARD_LIBS) \

LIBPATH = $(COMMON_LIB_PATHS)

SO_OBJS =  \
	./$(OBJ_DIR)/CLProfileAgent.o \
	./$(OBJ_DIR)/CLBuffer.o \
	./$(OBJ_DIR)/CLContext.o \
	./$(OBJ_DIR)/CLContextManager.o \
	./$(OBJ_DIR)/CLGPAProfiler.o \
	./$(OBJ_DIR)/CLKernel.o \
	./$(OBJ_DIR)/CLKernelAssembly.o \
	./$(OBJ_DIR)/ACLModuleManager.o \
	./$(OBJ_DIR)/CLUserEvent.o \
	./$(OBJ_DIR)/CLDeferredKernel.o \
	./$(OBJ_DIR)/CLProfilerMineCLEntry.o \
	./$(OBJ_DIR)/CLProfilerMineCLMemory.o \
	./$(OBJ_DIR)/GPUPerfAPICounterLoader.o \
	$(COMGRUTIL_SRC) \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
