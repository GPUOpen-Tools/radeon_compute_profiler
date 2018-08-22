#MAKE FILE FOR CLProfileAgent
PROJECT_NAME=CLProfileAgent
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETSO = $(OUTPUT_BIN_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).so

INCLUDES = \
	-I. \
	-I$(SRC_COMMON_DIR) \
	-I$(SRC_CLCOMMON_DIR) \
	-I$(COMMON_PROJ_DIR) \
	-isystem$(GPU_PERF_API_DIR) \
	-isystem$(APPSDK_DIR)/include \
	-isystem$(CAL_DIR)/include \
	-I$(DYNAMICLIBRARYMODULE_DIR) \
	-I$(DEVICEINFO_DIR) \
	-I$(TSINGLETON_DIR) \
	-isystem$(BOOST_DIR) \
	-I$(UTF8CPP_DIR) \
	-I$(ACL_DIR)/include \
	-I$(ADL_DIR)/include \
	-I$(ADLUTIL_DIR) \
	-I$(ACLMODULEMANAGER_DIR) \

LIBS = \
	$(CLCOMMON_LIBS) \
	$(COMMON_LIBS) \
	$(FRAMEWORK_LIBS) \
	$(STANDARD_LIBS)

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

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
