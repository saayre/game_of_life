/**
 * @file life.h
 * @author Sayre Christenson
 * @date 2013-04-21
 * @brief header file for the game of life
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h" 
#include "sdl.h"


#ifndef LIFE_H_
#define LIFE_H_


#define FILESIZE 100000
#define LINESIZE 100
#define FLAGSIZE 7


enum MAP_CASE {HEDGE, TORUS, KLEIN};


struct options_t {
        char *filename;
	char *filenameP;
	char *filenameQ;
        int width;
        int height;
        int size;
        int d1;
        int d2;
        int x0;
        int y0;
	int x0P;
	int y0P;
	int x0Q;
	int y0Q;
        int color;
        enum MAP_CASE sides;
};


struct options_t option_set(int argc, char *argv[]);
void options_help(void);
unsigned char ** make_map(struct options_t options);
void import_map(unsigned char **map, struct options_t options);
void neighbor_count(unsigned char **life, unsigned char **life_count, struct options_t options);
int hedge_count(unsigned char **life, int ind1, int ind2, struct options_t options);
int torus_count(unsigned char **life, int ind1, int ind2, struct options_t options);
int klein_count(unsigned char **life, int ind1, int ind2, struct options_t options);
void life_update(unsigned char **life, unsigned char **life_count, struct options_t options);
void free_map(unsigned char **map, struct options_t options);


#endif
