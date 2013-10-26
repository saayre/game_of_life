/**
 * @file gl.c
 * @edited_by Sayre Christenson
 * @date 2013-04-21
 * @brief main function for game of life
 */


#include "life.h"


int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	struct options_t options = option_set(argc, argv);

	// unpack options
	int width = options.width;    //first dimension
	int height = options.height;  //second dimension
	int color = options.color;
	int size = options.size;

        /* Initialize SDL's subsystems - in this case, only video. */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

        /* Register SDL_Quit to be called at exit; makes sure things are */
        /* cleaned up when we quit. */
	atexit(SDL_Quit);
    
        /* Attempt to create a screen with width x height window with 32bit pixels. */
	screen = SDL_SetVideoMode(width, options.height, 32, SDL_SWSURFACE);

        /* If SDL_SetVideoMode fails return error. */
	if ( screen == NULL ) 
	{
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}

	sdl_init(width, height, screen);

	/* your life initialization code here */

	unsigned char **life = make_map(options);
	unsigned char **life_count = make_map(options);
	import_map(life, options);

	/* change the color of the sprite */
	/* not used in sdl_test */
	

	/* Main loop: loop forever. */
	while (1) {

		/* change the modulus value to slow the rendering */
		if (SDL_GetTicks() % 100 == 0) {
			sdl_render_life(life, width, height, screen, color, size);
			neighbor_count(life, life_count, options);
			life_update(life, life_count, options);
		} 

		/* Poll for events, and handle the ones we care about. 
		 * You can click the X button to close the window
		 */
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
			{
				switch (event.type) 
					{
					case SDL_KEYDOWN:
						break;
					case SDL_KEYUP: 
						/* If escape is pressed, return (and thus, quit) */
						if (event.key.keysym.sym == SDLK_ESCAPE)
							return 0;
						break;
					case SDL_QUIT:
						free_map(life, options);
						free_map(life_count, options);
						return(0);
					}
			}
	} 
	return 0;
}
