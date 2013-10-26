/**
 * @file life.c
 * @author Sayre Christenson
 * @date 2013-04-21
 * @brief function definitions for the game of life
 */


#include "life.h"


/**
 * parses the command line and stores options in a structure
 * @param int argc, argument count
 * @param char *argv[], argument string
 * @param struct options_t *options, fresh options structure
 * @return struct options_t, option structure
 */
struct options_t option_set(int argc, char *argv[])
{
	struct options_t options;

	if (argc == 1) {
		printf("usage: life -HwhrgbsfoxyPpQqe \n");
		printf("-H is help.  All other options require arguments.\n");
		exit(EXIT_FAILURE);
	}

	// set defaults
	options.width = 800;  // first dimension
	options.height = 600; // second dimension
	options.size = 8;
	options.sides = TORUS;

	int red = 250;
	int green = 145;
	int blue = 250;
	
	/* Overly complicated yet awesometastic way of keeping flags.        */
	/* flag_sum gets multiplied by prime numbers (stored in flag_primes) */
	/* when certain conditions are met, and then its divibility is       */
	/* checked later to determine which flags were toggled.              */
	/* Flags that don't get toggled will force some action.              */
	/* HOW COULD THIS FAIL!?!?!                                          */
	int flags = 1;
	/* FLAGSIZE is 3 */
	int flag_primes[FLAGSIZE] = {2, 3, 5, 7, 11, 13, 17};
	/* 2 is flagged when setting filename     */
	/* 3 is flagged when setting x0           */
	/* 5 is flagged when setting y0           */

	/* 7 is flagged when setting filenameP    */
	/* 11 is flagged when setting x0P and y0P */

	/* 13 is flagged when setting filenameQ   */
	/* 17 is flagged when setting x0Q and y0Q */

	int tmp;
	int tmp2;
	char c;
	while ((c = getopt(argc, argv, ":Hw:h:r:g:b:s:f:o:x:y:P:p:Q:q:e:")) != -1) {
		switch (c) {
		case 'H' :
			options_help();
			exit(EXIT_SUCCESS);
			break;
		case 'w' :
			sscanf(optarg, "%d", &tmp);
			options.width = tmp; //first dimension
			break;

		case 'h' :
			sscanf(optarg, "%d", &tmp);
			options.height = tmp; // second dimension
			break;

		case 'r' :
			sscanf(optarg, "%d", &red);

			if (red < 0)
				red = 0;
			else if (red > 255)
				red = 255;
			break;

		case 'g' :
			sscanf(optarg, "%d", &green);

			if (green < 0)
				green = 0;
			else if (green > 255)
				green = 255;
			break;

		case 'b' :
			sscanf(optarg, "%d", &blue);

			if (blue < 0)
				blue = 0;
			else if (blue > 255)
				blue = 255;
			break;

		case 's' :
			sscanf(optarg, "%d", &tmp);
			switch (tmp) {
			case 2 :
			case 4 :
			case 8 :
			case 16 :
				options.size = tmp;
				break;
			default :
				fprintf(stderr, "wrong sprite size\n");
				exit(EXIT_FAILURE);
				break;
			}
			break;

		case 'f' :
			options.filename = optarg;
			flags *= flag_primes[0];
			break;

		case 'o' :
			// check if redundant
			if (flags % flag_primes[1] == 0) {
				fprintf(stderr, "-x and -o cannot both be used\n");
				exit(EXIT_FAILURE);
			}
			if (flags % flag_primes[2] == 0) {
				fprintf(stderr, "-y and -o cannot both be used\n");
				exit(EXIT_FAILURE);
			}

			sscanf(optarg, "%d,%d", &tmp, &tmp2);
			options.x0 = tmp;
			options.y0 = tmp2;
			flags *= flag_primes[1] * flag_primes[2];
			break;

		case 'x' :
			// check if redundant
			if (flags % flag_primes[1] == 0) {
				fprintf(stderr, "-x and -o cannot both be used\n");
				exit(EXIT_FAILURE);
			}

			sscanf(optarg, "%d", &tmp);
			options.x0 = tmp;
			flags *= flag_primes[1];
			break;

		case 'y' :
			// check if redundant
			if (flags % flag_primes[2] == 0) {
				fprintf(stderr, "-y and -o cannot both be used\n");
				exit(EXIT_FAILURE);
			}

			sscanf(optarg, "%d", &tmp);
			options.y0 = tmp;
			flags *= flag_primes[2];
			break;

		case 'P' :
			options.filenameP = optarg;
			flags *= flag_primes[3];
			break;

		case 'p' :
			sscanf(optarg, "%d,%d", &tmp, &tmp2);
			options.x0P = tmp;
			options.y0P = tmp2;
			flags *= flag_primes[4];
			break;

		case 'Q' :
			options.filenameQ = optarg;
			flags *= flag_primes[5];
			break;

		case 'q' :
			sscanf(optarg, "%d,%d", &tmp, &tmp2);
			options.x0Q = tmp;
			options.y0Q = tmp2;
			flags *= flag_primes[6];
			break;
			
		case 'e' :  // edge case
			/* checks first letter in argument only */
			switch (optarg[0]) {
			case 'h' :
			case 'H' :
				options.sides = HEDGE;
				break;

			case 't' :
			case 'T' :
				options.sides = TORUS;
				break;

			case 'k' :
			case 'K' :
				options.sides = KLEIN;
				break;

			default :
				fprintf(stderr, "Incorrect edge case.  Options are hedge, torus, and klein.\n");
				exit(EXIT_FAILURE);
				break;
			}
			break;

		case ':' :  // option missing argument
			fprintf(stderr, "%s: option '-%c' requires an argument\n",
				argv[0], optopt);
			exit(EXIT_FAILURE);
			break;

		case '?' :  // invalid option
			printf("wrong option -%c, ignoring\n", optopt);
		default :
			printf("usage: life -Hwhrgbsfoxye \n");
			printf("-H is help.  All other options require arguments.\n");
			break;
		}
	}

	// post-processing		
	options.d1 = options.width / options.size;
	options.d2 = options.height / options.size;
	options.color = sdl_set_color(red, green, blue); 

	// flagging magic!
	int i = 0;
	for ( ; i < FLAGSIZE ; i++) {
		if (flags % flag_primes[i] != 0) {
			
			switch (i) {
			case 0 :  // filename not set
				fprintf(stderr, "Filename not given.\nPlease set filename.\n");
				exit(EXIT_FAILURE);
				break;

			case 1 :  // x0 not set
				options.x0 = options.d1 / 2;
				break;

			case 2 :  // y0 not set
				options.y0 = options.d2 / 2;
				break;

			case 3 :  // filenameP not set
				options.filenameP = NULL;
				break;

			case 4 :  // x0P and y0P not set
				options.x0P = options.d1 / 2;
				options.y0P = options.d2 / 2;
				break;

			case 5 :  // filenameQ not set
				options.filenameQ = NULL;
				break;

			case 6 :  // x0Q and y0Q not set
				options.x0Q = options.d1 / 2;
				options.y0Q = options.d2 / 2;
				break;
			}

		}
	}
	
	return options;
}

/**
 * prints command line option meanings
 */
void options_help(void) {
	printf("\nusage: life -Hwhrgbsfoxye \n");
	printf("plays the game of life\n");
	printf("all options except -H require arguments\n\n");
	printf("H - prints this help display\n");
	printf("w - pixel width, default 800\n");
	printf("h - pixel height, default 600\n");
	printf("r - red RGB value, only 0-255, default 250\n");
	printf("g - green RGB value, only 0-255, default 145\n");
	printf("b - blue RGB value, only 0-255, default 250\n");
	printf("s - sprite size, only 2, 4, 8, or 16, default 8\n");
	printf("f - filename, REQUIRED\n");
	printf("o - initial x,y offset, syntax (-o X,Y), default same as x and y\n");
	printf("x - initial x offset, default halfway of screen\n");
	printf("y - initial y offset, default halfway of screen\n");
	printf("P - additional filename, optional\n");
	printf("p - initial x,y offset for file P, same as -o\n");
	printf("Q - additional filename, optional\n");
	printf("q - initial x,y offset for file Q, same as -o\n");
	printf("e - edge treatment, only hedge, torus, or klein, default torus\n");
}
	

/**
 * allocates memory for a grid map
 * @param struct options_t options, option structure
 * @return unsigned char **, double pointer to empty array
 */
unsigned char ** make_map(struct options_t options)
{
	// unpack options
	int d1 = options.d1;
	int d2 = options.d2;

	int i = 0;
	unsigned char **life = calloc(d1, sizeof(unsigned char *));
	if (life == NULL) {
		perror("Error with calloc: ");
		exit(EXIT_FAILURE);
	}

	for (i = 0 ; i < d1 ; i++) {
		life[i] = calloc(d2, sizeof(unsigned char));
		if (life[i] == NULL) {
			perror("Error with calloc: ");
			exit(EXIT_FAILURE);
		}
	}
	return life;
}


/**
 * initializes a map with a life 1.06 file
 * 1 for live cell, 0 for dead cell
 * @param unsigned char **map, double pointer to the game map
 * @param struct options_t options, option structure
 */
void import_map(unsigned char **map, struct options_t options)
{
	// unpack options
	char *filename = options.filename;
	int d1 = options.d1;
	int d2 = options.d2;
	int x0 = options.x0;
	int y0 = options.y0;

	FILE *init = fopen(filename, "r");
	if (init == NULL) {
		perror("Error opening file: ");
		exit(EXIT_FAILURE);
	}

	// read file into a string
	char c;
	int count = 0;
	char full_text[FILESIZE];
	int flag = 0;
	while ((c = fgetc(init)) != EOF) {
		full_text[count] = c;

		// skips first line
		if (flag == 1)
			count += 1;
		else if (c == '\n') 
			flag = 1;
	}
	full_text[count] = 0;
	fclose(init);
	
	char line[LINESIZE];
	int i;
	int j;
	count = 0;
	int line_index = 0;
	while ((c = full_text[count]) != 0) {
		if (c != '\n') {
			line[line_index] = c;
			line_index += 1;
		}
		else {
			sscanf(line, "%d %d", &j, &i);
			line_index = 0;

			// force indices inside array bounds, include initial positions
			i += x0;
			j += y0;
			while (i < 0) 
				i += d1;
			while (j < 0)
				j += d2;
			i = i % d1;
			j = j % d2;

			map[i][j] = 1;
		}
		count += 1;
	}

	// checks for additional files, not good coding style but very simple
	if (options.filenameP != NULL) {
		// unpack options
		char *filenameP = options.filenameP;
		x0 = options.x0P;
		y0 = options.y0P;

		FILE *initP = fopen(filenameP, "r");
		if (initP == NULL) {
			perror("Error opening file: ");
			exit(EXIT_FAILURE);
		}

		// read file into a string
		count = 0;
		flag = 0;
		while ((c = fgetc(init)) != EOF) {
			full_text[count] = c;

			// skips first line
			if (flag == 1)
				count += 1;
			else if (c == '\n') 
				flag = 1;
		}
		full_text[count] = 0;
		fclose(initP);
	
		count = 0;
		line_index = 0;
		while ((c = full_text[count]) != 0) {
			if (c != '\n') {
				line[line_index] = c;
				line_index += 1;
			}
			else {
				sscanf(line, "%d %d", &j, &i);
				line_index = 0;

				// force indices inside array bounds, include initial positions
				i += x0;
				j += y0;
				while (i < 0) 
					i += d1;
				while (j < 0)
					j += d2;
				i = i % d1;
				j = j % d2;

				map[i][j] = 1;
			}
			count += 1;
		}
	}

	if (options.filenameQ != NULL) {
		char *filenameQ = options.filenameQ;
		x0 = options.x0Q;
		y0 = options.y0Q;

		FILE *initQ = fopen(filenameQ, "r");
		if (initQ == NULL) {
			perror("Error opening file: ");
			exit(EXIT_FAILURE);
		}

		// read file into a string
		count = 0;
		flag = 0;
		while ((c = fgetc(init)) != EOF) {
			full_text[count] = c;

			// skips first line
			if (flag == 1)
				count += 1;
			else if (c == '\n') 
				flag = 1;
		}
		full_text[count] = 0;
		fclose(initQ);
	
		count = 0;
		line_index = 0;
		while ((c = full_text[count]) != 0) {
			if (c != '\n') {
				line[line_index] = c;
				line_index += 1;
			}
			else {
				sscanf(line, "%d %d", &j, &i);
				line_index = 0;

				// force indices inside array bounds, include initial positions
				i += x0;
				j += y0;
				while (i < 0) 
					i += d1;
				while (j < 0)
					j += d2;
				i = i % d1;
				j = j % d2;

				map[i][j] = 1;
			}
			count += 1;
		}
	}
}

/**
 * generates a map containing the number of live cells touching each cell
 * @param unsigned char **life, current live map
 * @param unsigned char **, life_count map
 * @param struct options_t options, option structure
 */
void neighbor_count(unsigned char **life, unsigned char **life_count, struct options_t options)
{
	// unpack options
	int d1 = options.d1;
	int d2 = options.d2;
	enum MAP_CASE sides = options.sides;
	
	int i, j;

	switch (sides) {
	case HEDGE :
		for (i = 0 ; i < d1 ; i++) {
			for (j = 0 ; j < d2 ; j++) 
				life_count[i][j] = hedge_count(life, i, j, options);
		}
		break;	
	case TORUS :
		for (i = 0 ; i < d1 ; i++) {
			for (j = 0 ; j < d2 ; j++) {
				life_count[i][j] = torus_count(life, i, j, options);
			}
		}
		break;
	case KLEIN :
		for (i = 0 ; i < d1 ; i++) {
			for (j = 0 ; j < d2 ; j++) {
				life_count[i][j] = klein_count(life, i, j, options);
			}
		}
		break;
	}
}

/**
 * counts the live neighbors for a given cell in the hedge case
 * 1 for live cell, 0 for dead cell
 * @param unsigned char **life, current game map
 * @param int ind1, first index
 * @param int ind2, second index
 * @param struct options_t options, option structure
 * @return int, count of live neighbors for the cell
 */
int hedge_count(unsigned char **life, int ind1, int ind2, struct options_t options)
{
	// unpack options
	int d1 = options.d1;
	int d2 = options.d2;

	int sum = 0;
	int i, j;

	for (i = ind1 - 1 ; i <= ind1 + 1 ; i++) {
		for (j = ind2 - 1 ; j <= ind2 + 1 ; j++) {
			if (i != ind1 || j != ind2) {
				
				// ignores cells outside the grid
				if (i < 0 || j < 0 || i >= d1 || j >= d2)
					continue;
				sum += life[i][j];
			}
		}
	}
	return sum;
}

/**
 * counts the live neighbors for a given cell in the torus case
 * 1 for live cell, 0 for dead cell
 * @param unsigned char **life, current game map
 * @param int ind1, first index
 * @param int ind2, second index
 * @param struct options_t options, option structure
 * @return int, count of live neighbors for the cell
 */
int torus_count(unsigned char **life, int ind1, int ind2, struct options_t options)
{
	// unpack options
	int d1 = options.d1;
	int d2 = options.d2;

	int sum = 0;
	int i, j;
	int i_check, j_check;

	for (i = ind1 - 1 ; i <= ind1 + 1 ; i++) {
		for (j = ind2 - 1 ; j <= ind2 + 1 ; j++) {
			if (!(i == ind1 && j == ind2)) {
				// sets each check outside the map to the other side
				if (i < 0)
					i_check = d1 - 1;
				else if (i == d1)
					i_check = 0;
				else
					i_check = i;

				if (j < 0)
					j_check = d2 - 1;
				else if (j == d2)
					j_check = 0;
				else
					j_check = j;

				sum += life[i_check][j_check];
			}
		}
	}
	return sum;
}

/**
 * counts the live neighbors for a given cell in the klein bottle case
 * 1 for live cell, 0 for dead cell
 * @param unsigned char **life, current game map
 * @param int ind1, first index
 * @param int ind2, second index
 * @param struct options_t options, option structure
 * @return int, count of live neighbors for the cell
 */
int klein_count(unsigned char **life, int ind1, int ind2, struct options_t options)
{
	// unpack options
	int d1 = options.d1;
	int d2 = options.d2;

	int sum = 0;
	int i, j;
	int i_check, j_check;

	for (i = ind1 - 1 ; i <= ind1 + 1 ; i++) {
		for (j = ind2 - 1 ; j <= ind2 + 1 ; j++) {
			if (!(i == ind1 && j == ind2)) {
				// connects top and bottom edge
				if (j < 0)
					j_check = d2 - 1;
				else if (j == d2)
					j_check = 0;
				else
					j_check = j;

				// connects left and right edge, resets j_check accordingly
				if (i < 0) {
					i_check = d1 - 1;
					j_check = (d2 - 1) - j_check;
				}
				else if (i == d1) {
					i_check = 0;
					j_check = (d2 - 1) - j_check;
				}
				else
					i_check = i;

				sum += life[i_check][j_check];
			}
		}
	}
	return sum;
}

/**
 * calculates the next generation in the game of life
 * 1 for live cell, 0 for dead cell
 * @param unsigned char **life, current game map
 * @param unsigned char **life_count, map containing neighbor info
 * @param struct options_t options, option structure
 */
void life_update(unsigned char **life, unsigned char **life_count, struct options_t options)
{
	// unpack options
	int d1 = options.d1;
	int d2 = options.d2;

	int i, j;
	for (i = 0 ; i < d1 ; i++) {
		for (j = 0 ; j < d2 ; j++) {

			// cell genesis
			if (life[i][j] == 0 && life_count[i][j] == 3)
				life[i][j] = 1;

			// cell death
			else if (life[i][j] == 1 && (life_count[i][j] < 2 || life_count[i][j] > 3))
				life[i][j] = 0;
		}
	}
}

/**
 * fully frees a map
 * @param unsigned char **map, double pointer to map
 * @param struct options_t options, option structure
 */
void free_map(unsigned char **map, struct options_t options)
{
	// unpack options
	int d1 = options.d1;

	int i;
	for (i = 0 ; i < d1 ; i++) 
		free(map[i]);
	free(map);
}
