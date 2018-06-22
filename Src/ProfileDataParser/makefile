#MAKE FILE FOR RCPProfileDataParser
PROJECT_NAME=ProfileDataParser
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

TARGETSO = $(OUTPUT_BIN_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).so

INCLUDES = \
	-I./ \
	-I$(COMMON_SRC)\
	-I$(SRC_COMMON_DIR) \
	-I$(SRC_CLCOMMON_DIR) \
	-I$(TSINGLETON_DIR) \
	-I$(HSAUTILS_DIR) \
	-I$(APPSDK_DIR)/include \
	-I$(SRC_HSAFDNCOMMON_DIR) \
	-I$(DEVICEINFO_DIR)

LIBPATH = $(COMMON_LIB_PATHS)

LIBS = \
	$(CLCOMMON_LIBS) \
	$(COMMON_LIBS) \
	$(FRAMEWORK_LIBS) \
	$(STANDARD_LIBS) \
	$(LOCAL_HSAUTILS_LIBS)

SO_OBJS = \
	./$(OBJ_DIR)/AtpDataHandlerImp.o \
	./$(OBJ_DIR)/ATPParserImp.o \
	./$(OBJ_DIR)/CLApiInfoDataHandlerImp.o \
	./$(OBJ_DIR)/HSAApiInfoDataHandlerImpl.o \
	./$(OBJ_DIR)/OccupancyFileInfoDataHandlerImp.o \
	./$(OBJ_DIR)/OccupancyInfoDataHandlerImp.o \
	./$(OBJ_DIR)/PerfMarkerInfoDataHandlerImp.o \
	./$(OBJ_DIR)/SymbolFileEntryInfoDataHandlerImp.o \
	./$(OBJ_DIR)/CLAtpFile.o \
	./$(OBJ_DIR)/StackTraceAtpFile.o \
	./$(OBJ_DIR)/HSAFunctionDefsUtils.o \
	./$(OBJ_DIR)/HSAAtpFile.o \
	./$(OBJ_DIR)/AtpFile.o \
	./$(OBJ_DIR)/OccupancyChart.o \
	./$(OBJ_DIR)/OccupancyUtils.o \
	./$(OBJ_DIR)/KernelProfileResultParser.o\
	./$(OBJ_DIR)/CSVFileParser.o\
	./$(OBJ_DIR)/PerfMarkerAtpFile.o \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
