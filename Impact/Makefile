CC=gcc
CXX=g++
RM=rm -f
CFLAGS=-g
CXXFLAGS=-g -std=c++11 -DNO_RECORDER -DLINUX_AMD64
LDFLAGS=-g
LDLIBS=-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -lm -lGLEW -lGL -lz -lBox2D -lboost_serialization -lboost_regex -lX11

SRCS=Ball.cpp Block.cpp Body.cpp Bumper.cpp Explosion.cpp globals.cpp Ground.cpp \
     Impact.cpp Level.cpp LocalSettings.cpp main.cpp Racket.cpp \
     sha1.cpp stdafx.cpp Text.cpp util.cpp Wall.cpp

MINIZIP_SRCS=../minizip/unzip.c ../minizip/miniunz.c ../minizip/ioapi.c

OBJS=$(subst .cpp,.o,$(SRCS))
MINIZIP_OBJS=$(subst .c,.o,$(MINIZIP_SRCS))

all: impact

impact: $(OBJS) $(MINIZIP_OBJS)
	$(CXX) $(LDFLAGS) -o impact $(OBJS) $(MINIZIP_OBJS) $(LDLIBS) 

clean:
	rm *.o ../minizip/*.o impact
