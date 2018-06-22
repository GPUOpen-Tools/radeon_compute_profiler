#MAKE FILE FOR CLOccupancyAgent
PROJECT_NAME=CLOccupancyAgent
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETSO = $(OUTPUT_BIN_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).so

INCLUDES = \
	-I. \
	-I$(SRC_CLPROFILEAGENT_DIR) \
	-I$(SRC_CLTRACEAGENT_DIR) \
	-I$(SRC_COMMON_DIR) \
	-I$(SRC_CLCOMMON_DIR) \
	-I$(COMMON_PROJ_DIR) \
	-I$(APPSDK_DIR)/include \
	-I$(CAL_DIR)/include \
	-I$(DYNAMICLIBRARYMODULE_DIR) \
	-I$(DEVICEINFO_DIR) \
	-I$(TSINGLETON_DIR) \
	-I$(UTF8CPP_DIR)

LIBS = \
	$(CLCOMMON_LIBS) \
	$(COMMON_LIBS) \
	$(FRAMEWORK_LIBS) \
	$(STANDARD_LIBS)

LIBPATH = $(COMMON_LIB_PATHS)

SO_OBJS =  \
	./$(OBJ_DIR)/CLIntercept.o \
	./$(OBJ_DIR)/CLOccupancyAgent.o \
	./$(OBJ_DIR)/CLOccupancyFile.o \
	./$(OBJ_DIR)/CLOccupancyInfoManager.o \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
