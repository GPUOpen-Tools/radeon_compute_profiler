#MAKE FILE FOR PreloadXInitThreads
PROJECT_NAME=PreloadXInitThreads
DEPTH = ../..
include $(DEPTH)/Build/Linux/Common.mk

# make sure CallXInitThreads gets called during initialization
ADDL_LINKFLAGS = -Wl,-init,CallXInitThreads
TARGETSO = $(OUTPUT_BIN_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)$(PROJECT_NAME)$(TARGET_SUFFIX).so

INCLUDES = \
	-I. \

LIBS = -lX11

LIBPATH = $(COMMON_LIB_PATHS)

SO_OBJS =  \
	./$(OBJ_DIR)/XInitThreadsPreload.o \

include $(DEPTH)/Build/Linux/CommonTargets.mk

# END OF MAKE FILE
