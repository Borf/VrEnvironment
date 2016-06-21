#!make

export

# Target folders
TOPDIR := $(shell echo `pwd`)

# Compiler flags
LIBS = -lGL
INCLUDES = -Isrc -DGRF_STATIC -D__NOXML__ -DGLEW_STATIC \
	-I../../Libraries/VrLib/ \
	-I../../Libraries/VrLib/externals/glm \
	-I../../Libraries/BulletPhysics/bullet3/src
	
CXX = g++
CC = gcc
DEFINES = 
CFLAGS = -Wall -Wno-unknown-pragmas -pipe -std=c++0x
#CPPFLAGS = -Wall -pipe -std=c++0x

# where are source files located?
SOURCE_DIRS=.

# Host platform
UNAME=$(shell uname -s | sed -e 's/_.*$$//')

#####
## PLATFORM DETECTION CODE
#####
ifeq ($(PLATFORM),)
UNAME_CPU=$(shell uname -m)

## Cygwin
ifeq ($(UNAME),CYGWIN)
# can't do linux build anyway on cygwin
PLATFORM=win32
endif

## Linux
ifeq ($(UNAME),Linux)
# detect 64bit
ifeq ($(UNAME_CPU),x86_64)
PLATFORM=linux64
else
PLATFORM=linux32
endif
endif

endif

#####
## END OF PLATFORM DETECTION CODE
#####

#####
## PER-PLATFORM SETTINGS
#####

# Linux 32bit
ifeq ($(PLATFORM),linux32)
CFLAGS += -m32
CC=gcc
CXX=g++
BINARY_EXT=
LIBRARY_EXT=.so
LIBRARY_PREFIX=lib
LIBS = ../../Libraries/VrLib/VrLib_linux32.a \
../../Libraries/BulletPhysics/bullet3/build/src/Bullet3Common/libBullet3Common.a \
../../Libraries/BulletPhysics/bullet3/build/src/BulletDynamics/libBulletDynamics.a \
../../Libraries/BulletPhysics/bullet3/build/src/BulletCollision/libBulletCollision.a \
../../Libraries/BulletPhysics/bullet3/build/src/LinearMath/libLinearMath.a \
-lGL -lGLEW -lpthread -lassimp -L.
endif


#../../Libraries/BulletPhysics/bullet3/build/src/Bullet3Collision/libBullet3Collision.a \
#../../Libraries/BulletPhysics/bullet3/build/src/Bullet3Dynamics/libBullet3Dynamics.a \
#../../Libraries/BulletPhysics/bullet3/build/src/Bullet3Geometry/libBullet3Geometry.a \
#../../Libraries/BulletPhysics/bullet3/build/src/Bullet3OpenCL/libBullet3OpenCL.a \
#../../Libraries/BulletPhysics/bullet3/build/src/Bullet3Serialize/libBullet3Serialize.a \
#../../Libraries/BulletPhysics/bullet3/build/src/BulletInverseDynamics/libBulletInverseDynamics.a \
#../../Libraries/BulletPhysics/bullet3/build/src/BulletSoftBody/libBulletSoftBody.a \

# Linux 64bit
ifeq ($(PLATFORM),linux64)
CFLAGS += -m64
CC=gcc
CXX=g++
BINARY_EXT=
LIBRARY_EXT=.so
LIBRARY_PREFIX=lib
endif

# Windows 32bit
ifeq ($(PLATFORM),win32)
# Mh, we don't use GUI only mode, but console instead?
CFLAGS += -mconsole
ifeq ($(UNAME),CYGWIN)
# On cygwin, use provided gcc, but tell to not use cygwin's dlls
CC=gcc
CXX=g++
CFLAGS += -mno-cygwin -DNEEDS_DLLMAIN
else
DEBIAN=$(shell which i586-mingw32msvc-g++)
ifeq ($(DEBIAN),/usr/bin/i586-mingw32msvc-g++)
CC=i586-mingw32msvc-gcc
CXX=i586-mingw32msvc-g++
AR=i586-mingw32msvc-ar
else
CC=mingw32-gcc
CXX=mingw32-g++
AR=mingw32-ar
endif

endif
WINDRES=mingw32-windres
BINARY_EXT=.exe
LIBRARY_EXT=.dll
LIBRARY_PREFIX=
LIBS = -L. -Llibs/lib -lzlib1 -lSDL -lbgd -lopengl32 -lglu32 -lws2_32 -lcomdlg32 -lcurl -lSDL_mixer -lwinmm -lgdi32
# ws2_32.lib sdl.lib sdlmain.lib zlib.lib bgd.lib opengl32.lib glu32.lib 
endif

#####
## END OF PER-PLATFORM SETTINGS
#####

## Debug build?
ifeq ($(DEBUG),)
DEBUG=yes
endif

ifeq ($(DEBUG),yes)
CFLAGS += -g -ggdb -D_DEBUG
else
# We suppose everyone have SSE. If this cause problems, switch mfpmath back to 387
CFLAGS += -O3 -fomit-frame-pointer -momit-leaf-frame-pointer -mfpmath=387
endif

TARGET=TienTest_$(PLATFORM)$(BINARY_EXT)

# Define path where make will find source files :)
VPATH=$(subst  ,:,$(SOURCE_DIRS))

# list all source files
SOURCE_ALL=$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c $(dir)/*.cpp))

# list all object files
OBJECTS_ALL=$(patsubst %,obj/%_$(PLATFORM).o,$(notdir $(basename $(SOURCE_ALL))))

# list all .dep files
DEP_ALL=$(patsubst %,obj/%_$(PLATFORM).dep,$(notdir $(basename $(filter %.c %.cpp,$(SOURCE_ALL)))))

ifeq ($(PLATFORM),win32)
# Fix: Win32 build needs this one
OBJECTS_SRC += obj/src_Script1_rc_$(PLATFORM).o
endif

all: $(TARGET)

bengine:
	make -C libs/bengine
	@cp `find libs/bengine -name "*.a"` libs/bengine


# Depencies

dep: obj/depencies_$(PLATFORM).mak

obj/depencies_$(PLATFORM).mak: $(DEP_ALL)
	@echo -e "    \033[1mDEP\033[1m\t\033[22;34m$@\033[39m                           "
# build depencies for objects
	@cat $^ | sed -r -e 's#^([a-zA-Z0-9]+)\.o#obj/\1_$(PLATFORM).o#' >$@
# build depencies for depencies
	@cat $^ | sed -r -e 's#^([a-zA-Z0-9]+)\.o#obj/\1_$(PLATFORM).dep#' >>$@

#-include obj/depencies_$(PLATFORM).mak

# type-specific targets (compile, target = %.o)

obj/%_$(PLATFORM).o: %.c
	@echo -e "    \033[1mCC\033[1m\t\033[22;34m$<\033[39m"
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

obj/%_rc_$(PLATFORM).o: %.rc
	@echo -e "    \033[1mRC\033[1m\t\033[22;34m$<\033[39m"
	@$(WINDRES) -I src -i $< -o $@

obj/%_$(PLATFORM).o: %.cpp
	@echo -e "    \033[1mCC\033[1m\t\033[22;34m$<\033[39m"
	@$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# depencies

obj/%_$(PLATFORM).dep: %.c
	@echo -en "    \033[1mDEP\033[1m\t\033[22;34m$<\033[39m                           \r"
	@$(CC) $(CFLAGS) $(INCLUDES) -MM -MF $@ $<

obj/%_$(PLATFORM).dep: %.cpp
	@echo -en "    \033[1mDEP\033[1m\t\033[22;34m$<\033[39m                           \r"
	@$(CXX) $(CFLAGS) $(INCLUDES) -MM -MF $@ $<

# Main target

$(TARGET): $(OBJECTS_ALL)
	@echo -e "    \033[1mLD\033[1m\t\033[22;35m$@\033[39m"
	@$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

#NEWTARGET#

