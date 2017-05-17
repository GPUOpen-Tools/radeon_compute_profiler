#MAKE FILE FOR DeviceInfo
PROJECT_NAME=DeviceInfo
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETLIB = $(OUTPUT_LIB_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).a

INCLUDES = \
	-I. \
	-I../Common \
	-I$(COMMON_PROJ_DIR) \
	-I$(DEVICEINFO_DIR) \
	-I$(TSINGLETON_DIR) \

# for AMD-internal builds
ifneq ("$(wildcard $(DEVICEINFO_INTERNAL_DIR)/DeviceInfoInternal.cpp)","")
	INTERNAL_OBJ := ./$(OBJ_DIR)/DeviceInfoInternal.o
endif

LIB_OBJS = \
	./$(OBJ_DIR)/DeviceInfoUtils.o \
	$(INTERNAL_OBJ) \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
