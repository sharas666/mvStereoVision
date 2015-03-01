# name of program
PROG = main

#compiler flags
CC = g++
CFLAGS  = -c -fPIC -Wall -fdiagnostics-color=auto -std=c++11

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

all: directories $(OBJ)
	@$(CC) $(OBJ) -o $(BINDIR)/$(PROG) $(LIB_PATH) $(LIBS)
	@echo Linking complete!

directories:
	${MKDIR_P} ${OBJDIR}
	${MKDIR_P} ${BINDIR}
	
$(OBJ): $(OBJDIR)/%.o :$(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) $(INC_PATH) $< -o $@
	@echo Compiled $< $ successfully!

clean:
	$(RM) $(OBJDIR) $(BINDIR)

#To compile mvIMPACTacquire SDK with c++11 find the file:
#/op/mvIMPACT_acquire/mvPropHandling/include/mvPropHandlingDatatypes.h
#and change the line (near line 27)
#   elif defined(linux)
#to
#    elif defined(__linux__)


