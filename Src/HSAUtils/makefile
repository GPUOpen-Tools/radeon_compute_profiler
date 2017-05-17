#MAKE FILE FOR HSAUtils
PROJECT_NAME=HSAUtils
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETLIB = $(OUTPUT_LIB_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).a

INCLUDES = \
	-isystem$(HSA_DIR)/include \
	-isystem$(HSA_DIR)/include/hsa \
	-I$(TSINGLETON_DIR) \
	-I$(HSAUTILS_DIR) \
	-I$(DYNAMICLIBRARYMODULE_DIR)

LIB_OBJS = \
	./$(OBJ_DIR)/HSAUtils.o \
	./$(OBJ_DIR)/HSAModule.o \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
