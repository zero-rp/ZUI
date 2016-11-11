CXXFLAGS = -O3 -I./ZUI -I./ZUI/core/agg/include
CFLAGS = -O3 -std=c99 -Wimplicit-function-declaration -I./ZUI 
CXX = g++
C = gcc
LIB = ar cr
LD = ld

.PHONY : clean

SRC_CXX=\
	ZUI/core/agg/src/agg_curves.cpp \
	ZUI/core/agg/src/agg_trans_affine.cpp \
	ZUI/core/agg/src/agg_vcgen_stroke.cpp


SRC_C=\
	ZUI/debug.c \
	ZUI/core/carray.c \
    ZUI/core/tree.c 


OBJ=$(SRC_CXX:.cpp=.o) $(SRC_C:.c=.o)

build:	$(OBJ)
	$(CXX) $(OBJ) -o bin/Linux/Debug/ZUI

clean:


%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $*.cpp -o $@
%.o: %.c
	$(C) -c $(CFLAGS) $*.c -o $@