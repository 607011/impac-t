CC = gcc
CXX = g++
RM = rm -f
PROJECTS = \
	../minizip
DEBUGFLAGS = -DDEBUG -ggdb
RELEASEFLAGS = -DNDEBUG -O3
GTKCFLAGS=$(shell pkg-config gtk+-3.0 --cflags)
GTKLIBS=$(shell pkg-config gtk+-3.0 --libs)
CFLAGS = -pthread
CXXFLAGS = $(GTKCFLAGS) -pthread -std=c++11 -DNO_RECORDER -DLINUX_AMD64
LDFLAGS = 
LDLIBS = $(GTKLIBS) -pthread -lsfml-graphics -lsfml-window -lsfml-audio	\
     -lsfml-system -lm -lGLEW -lGL -lz -lBox2D -lboost_serialization	\
     -lboost_regex -lX11 -lboost_system -lboost_filesystem

SRCS = Ball.cpp Block.cpp Body.cpp Bumper.cpp Explosion.cpp		\
     globals.cpp Ground.cpp Impact.cpp Level.cpp LocalSettings.cpp	\
     main.cpp Racket.cpp sha1.cpp stdafx.cpp Text.cpp util.cpp		\
     Wall.cpp linux_amd64.cpp

MINIZIP_SRCS = ../minizip/unzip.c ../minizip/miniunz.c	\
../minizip/ioapi.c

OBJS=$(subst .cpp,.o,$(SRCS))
MINIZIP_OBJS=$(subst .c,.o,$(MINIZIP_SRCS))

all: release

debug:
	$(MAKE) impact CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS) $(DEBUGFLAGS)" CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" LDFLAGS="$(LDFLAGS)"

release:
	$(MAKE) impact CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS) $(RELEASEFLAGS)" CXXFLAGS="$(CXXFLAGS) $(RELEASEFLAGS)" LDFLAGS="$(LDFLAGS)"


impact: $(OBJS) $(MINIZIP_OBJS)
	$(CXX) $(LDFLAGS) -o impact $(OBJS) $(MINIZIP_OBJS) $(LDLIBS) 

clean:
	$(RM) *.o ../minizip/*.o impact
