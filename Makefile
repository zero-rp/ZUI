CXXFLAGS = -O3 -I./ZUI -I./ZUI/platform/graph/agg/include -I./ZUI/platform
CFLAGS = -O3 -std=c99 -Wimplicit-function-declaration -I./ZUI -I./ZUI/zlib -I./ZUI/platform
CXX = g++
C = gcc
LIB = ar cr
LD = ld

.PHONY : clean

SRC_CXX=\
	ZUI/platform/graph/agg/src/agg_curves.cpp \
	ZUI/platform/graph/agg/src/agg_trans_affine.cpp \
	ZUI/platform/graph/agg/src/agg_vcgen_stroke.cpp


SRC_C=\
	ZUI/core/carray.c \
	ZUI/core/tree.c \
	ZUI/core/control.c \
	ZUI/core/builder.c \
	ZUI/core/function.c \
	ZUI/core/global.c \
	ZUI/core/mxml.c \
	ZUI/zlib/adler32.c \
	ZUI/zlib/crc32.c \
	ZUI/zlib/inffast.c \
	ZUI/zlib/inflate.c \
	ZUI/zlib/inftrees.c \
	ZUI/zlib/zutil.c \
	ZUI/layout/HorizontalLayout.c \
	ZUI/layout/Layout.c \
	ZUI/layout/TabLayout.c \
	ZUI/layout/TileLayout.c \
	ZUI/layout/VerticalLayout.c \
	ZUI/zuidebug.c  


OBJ=$(SRC_CXX:.cpp=.o) $(SRC_C:.c=.o)

build:	$(OBJ)
	$(CXX) $(OBJ) -o bin/Linux/Debug/ZUI

clean:


%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $*.cpp -o $@
%.o: %.c
	$(C) -c $(CFLAGS) $*.c -o $@