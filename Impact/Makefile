CC = gcc
CXX = g++
RM = rm -f
CFLAGS = -pthread
CXXFLAGS = -pthread -std=c++11 -DNO_RECORDER -DLINUX_AMD64
LDFLAGS =
LDLIBS = -pthread -lsfml-graphics -lsfml-window -lsfml-audio	\
-lsfml-system -lm -lGLEW -lGL -lz -lBox2D -lboost_serialization	\
-lboost_regex -lX11

SRCS = Ball.cpp Block.cpp Body.cpp Bumper.cpp Explosion.cpp		\
     globals.cpp Ground.cpp Impact.cpp Level.cpp LocalSettings.cpp	\
     main.cpp Racket.cpp sha1.cpp stdafx.cpp Text.cpp util.cpp		\
     Wall.cpp

MINIZIP_SRCS = ../minizip/unzip.c ../minizip/miniunz.c	\
../minizip/ioapi.c

OBJS=$(subst .cpp,.o,$(SRCS))
MINIZIP_OBJS=$(subst .c,.o,$(MINIZIP_SRCS))

debug: CXXFLAGS += -ggdb -DDEBUG
debug: CFLAGS += -ggdb

release: CXXFLAGS += -DNDEBUG -O3
release: CFLAGS += -DNDEBUG -O3
release: LDFLAGS += -O3

release: all

all: impact

impact: $(OBJS) $(MINIZIP_OBJS)
	$(CXX) $(LDFLAGS) -o impact $(OBJS) $(MINIZIP_OBJS) $(LDLIBS) 

clean:
	$(RM) *.o ../minizip/*.o impact
