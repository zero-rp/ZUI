CXXFLAGS = -O3 -I./ZUI
CFLAGS = -O3 -std=c99 -I./ZUI
CXX = g++
C = gcc
LIB = ar cr
LD = ld

.PHONY : clean

SRC_CXX=


SRC_C=\
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