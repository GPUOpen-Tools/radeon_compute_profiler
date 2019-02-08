#MAKE FILE FOR HSACommon
PROJECT_NAME=HSACommon
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETLIB = $(OUTPUT_LIB_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).a

ADDL_DEFINES = -DAMD_INTERNAL_BUILD
ADDL_CFLAGS = $(HSAFDN_CFLAGS)

INCLUDES = \
	-I. \
	-I../Common \
	-I$(COMMON_PROJ_DIR) \
	-I$(HSA_DIR)/include \
	-I$(HSA_DIR)/include/hsa \
	-I$(HSA_DIR)/../include \
	-I$(HSA_DIR)/../include/rocprofiler \
	-I$(DYNAMICLIBRARYMODULE_DIR) \
	-I$(TSINGLETON_DIR) \

LIB_OBJS =  \
	./$(OBJ_DIR)/HSAFunctionDefsUtils.o \
	./$(OBJ_DIR)/HSAModule.o \
	./$(OBJ_DIR)/ROCProfilerModule.o \
	./$(OBJ_DIR)/HSAAgentUtils.o \
	./$(OBJ_DIR)/HSAAgentIterateReplacer.o \
	./$(OBJ_DIR)/HSAKernelDemangler.o \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
