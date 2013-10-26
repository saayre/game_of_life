CC=gcc
CFLAGS=-Wall -g
SDL_CFLAGS := $(shell sdl-config --cflags) 
SDL_LDFLAGS := $(shell sdl-config --libs) -lm 

all: life.o gl

life.o: life.c life.h
	$(CC) $(CFLAGS) -c life.c

gl: gl.c life.o
	$(CC) $(CFLAGS) $(SDL_CFLAGS) gl.c sdl.o life.o -o life $(SDL_LDFLAGS)

clean:
	rm life life.o
