#MAKE FILE FOR Common
PROJECT_NAME=Common
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETLIB = $(OUTPUT_LIB_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).a

INCLUDES = \
	-I. \
	-I../Common \
	-I$(COMMON_PROJ_DIR) \
	-I$(DYNAMICLIBRARYMODULE_DIR) \
	-I$(DEVICEINFO_DIR) \
	-I$(TSINGLETON_DIR) \
	-I$(UTF8CPP_DIR) \
	-I$(GPU_PERF_API_DIR) \
	-I$(ACL_DIR)/include \
	-I$(ADL_DIR)/include

LIB_OBJS =  \
	./$(OBJ_DIR)/APIInfoManagerBase.o \
	./$(OBJ_DIR)/APITraceUtils.o \
	./$(OBJ_DIR)/ATPFileUtils.o \
	./$(OBJ_DIR)/TraceInfoManager.o \
	./$(OBJ_DIR)/OSUtils.o \
	./$(OBJ_DIR)/FileUtils.o \
	./$(OBJ_DIR)/GlobalSettings.o \
	./$(OBJ_DIR)/Logger.o \
	./$(OBJ_DIR)/LocaleSetting.o \
	./$(OBJ_DIR)/BinFileHeader.o \
	./$(OBJ_DIR)/StackTracer.o \
	./$(OBJ_DIR)/HTMLTable.o \
	./$(OBJ_DIR)/jqPlotChart.o \
	./$(OBJ_DIR)/StringUtils.o \
	./$(OBJ_DIR)/DynamicLibraryModule.o \
	./$(OBJ_DIR)/KernelStats.o \
	./$(OBJ_DIR)/GPAUtils.o \
	./$(OBJ_DIR)/CSVFileParser.o \
	./$(OBJ_DIR)/KernelProfileResultManager.o \
	./$(OBJ_DIR)/ACLModule.o \
	./$(OBJ_DIR)/ADLUtil.o \
	./$(OBJ_DIR)/ProfilerTimer.o \
	./$(OBJ_DIR)/GPUPerfAPICounterLoader.o


include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
