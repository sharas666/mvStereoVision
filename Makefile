# name of program
TARGETDIR = trgt
TARGETOBJDIR = trgt_obj
#compiler flags
CC = g++
CFLAGS  = -c -g -fPIC -Wall -fdiagnostics-color=auto -std=c++11

#source directories
SRCDIR = src

#binary directories
OBJDIR = obj
BINDIR = bin

#soruce files
SRC = $(wildcard $(SRCDIR)/*.cpp)

#obj files
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

#commands
MKDIR_P = mkdir -p
RM = rm -rf

#include path
INC_PATH = -I./inc/  -I/opt/mvIMPACT_acquire/

#library paths
LIB_PATH = -L/opt/mvIMPACT_acquire/lib/x86_64/

#libraries
OPENCV = `pkg-config opencv --cflags --libs`
LIBS = -lmvDeviceManager -lpthread $(OPENCV)


capture: directories $(OBJ)
	@$(CC) $(CFLAGS) $(INC_PATH) $(TARGETDIR)/capture.cpp -o $(TARGETOBJDIR)/capture.o
	@echo Compiled capture.cpp successfully!
	@$(CC) $(OBJ) $(TARGETOBJDIR)/capture.o -o $(BINDIR)/capture $(LIB_PATH) $(LIBS)
	@echo Linking complete!

epipoles: directories $(OBJ)
	@$(CC) $(CFLAGS) $(INC_PATH) $(TARGETDIR)/epipolarLines.cpp -o $(TARGETOBJDIR)/epipolarLines.o
	@echo Compiled epipolarLines.cpp successfully!
	@$(CC) $(OBJ) $(TARGETOBJDIR)/epipolarLines.o -o $(BINDIR)/epipolarLines $(LIB_PATH) $(LIBS)
	@echo Linking complete!

disparity: directories $(OBJ)
	@$(CC) $(CFLAGS) $(INC_PATH) $(TARGETDIR)/liveDisparity.cpp -o $(TARGETOBJDIR)/liveDisparity.o
	@$(CC) $(OBJ) $(TARGETOBJDIR)/liveDisparity.o -o $(BINDIR)/liveDisparity $(LIB_PATH) $(LIBS)
	@echo Linking complete!

directories:
	${MKDIR_P} ${OBJDIR}
	${MKDIR_P} ${BINDIR}
	${MKDIR_P} ${TARGETOBJDIR}

	
$(OBJ): $(OBJDIR)/%.o :$(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) $(INC_PATH) $< -o $@
	@echo Compiled $< $ successfully!

clean:
	$(RM) $(OBJDIR) $(BINDIR) $(TARGETOBJDIR)

#To compile mvIMPACTacquire SDK with c++11 find the file:
#/opt/mvIMPACT_acquire/mvPropHandling/include/mvPropHandlingDatatypes.h
#and change the line (near line 27)
#   elif defined(linux)
#to
#    elif defined(__linux__)


