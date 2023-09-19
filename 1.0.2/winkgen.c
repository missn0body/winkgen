#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// So, a primitive is composed of bits, 8 bits
// to a byte, and a character is (usually) guarenteed
// to be a single byte. A bit can either be on or off.
// Which means, with a single character, you've got
// a set of 8 booleans to use as flags, which I think
// is really really neat.

#define VERBOSE		(1 << 1)
#define OEMMODE		(1 << 2)
#define CDMODE		(1 << 3)
#define MULTI		(1 << 4)
#define ANSI		(1 << 5)

// ANSI escape codes, to use for color output, cause
// I quite like colors in my terminal :)
#define RED		"\033[30;101m"
#define BLUE    	"\033[34m"
#define GREEN   	"\033[32m"
#define RESET		"\033[0m"

// We're not using "char", just in case there's some
// dystopian future where they changed the definition
// of "char". Might be just bias, but I put more trust
// in stdint.h's typedefs.
typedef uint8_t flag_t;
static flag_t status = 0;

const char *errorMes[] =
{
	"inexplicable",
	"too few arguments",
	"unknown option",
	"missing option argument",
	"no number given"
};

// Quick mnemonics for indexing the above array.
enum { INX = 0, ARG, UNK, MOA, NNG };

// This function is not meant to be called directly, unless you
// want to provide a file and line number directly instead of
// nifty GCC extensions.
static inline void _error(char *file, int line, int index, flag_t *status)
{
	if((*status & ANSI) != 0) fprintf(stderr, "%s(%s:%d)%s %s\n", RED, file, line, RESET, errorMes[index]);
	else			  fprintf(stderr, "(%s:%d) %s\n", file, line, errorMes[index]);
	return;
}

// Rather I prefer you call this, unless you can't.
#define error(x)	_error(__FILE__, __LINE__, x, &status)

//////////////////////////////////////////////////////////////
// IMPORTANT FUNCTIONS BEGIN HERE
//////////////////////////////////////////////////////////////

void genOEM(void) { return; }
void genCD(void) { return; }

//////////////////////////////////////////////////////////////
// IMPORTANT FUNCTIONS END HERE
//////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	if(argc < 2) { error(ARG); exit(EXIT_FAILURE); }
	int c;
	int times = 0;

	// Iterate through all arguments sent, while making sure
	// that they start with a dash.
	while(--argc > 0 && (*++argv)[0] == '-')
	{
		// If there's another dash, then it's a long option.
		// Move the pointer up 2 places and compare the word itself.
		if((*argv)[1] == '-')
		{
			// Using continue statements here so that the user
			// can use both single character and long options
			// simultaniously, and the loop can test both.
			if(strcmp((*argv) + 2, "verbose") == 0) { status |= VERBOSE; continue; }
			if(strcmp((*argv) + 2, "oem") 	  == 0) { status |= OEMMODE; continue; }
			if(strcmp((*argv) + 2, "cd") 	  == 0) { status |= CDMODE; continue; }
			if(strcmp((*argv) + 2, "ansi") 	  == 0) { status |= ANSI; continue; }
			// The "multi" or "m" option requires a number.
			// Let's make sure that we get that number.
			if(strcmp((*argv) + 2, "multi") == 0)
			{
				char buffer[20];
				printf("How many keys to generate? : ");

				if(fgets(buffer, sizeof(buffer), stdin) == NULL || buffer[0] == '\r' || buffer[0] == '\n')
				{
					error(NNG);
					// We'll allow the program to look for more arguments,
					// If you don't put a number here, then the program will
					// simply ignore your input.
					continue;
				}
				sscanf(buffer, "%d", &times);
				continue;
			}
		}
		while((c = *++argv[0]))
		{
			// Single character option testing here.
			switch(c)
			{
				case 'v': status |= VERBOSE; break;
				case 'o': status |= OEMMODE; break;
				case 'c': status |= CDMODE;  break;
				//case 'm': status |= MULTI;   break;
				case 'a': status |= ANSI;    break;
				// This error flag can either be set by a
				// completely unrelated character inputted,
				// or you managed to put -option instead of
				// --option.
				default : error(UNK); exit(EXIT_FAILURE);
			}
		}
	}

	// whoops you get lots of info now
	if((status & VERBOSE) != 0)
	{
		if((status & ANSI) != 0) printf("%sANSI mode is on.\n%s", GREEN, RESET);
		printf("%sVerbose flag is set.%s\n", (status & ANSI) != 0 ? GREEN : "",
						     (status & ANSI) != 0 ? RESET : "");
	}

	// If you don't specify whether you want an OEM key or CD key,
	// Then you get both.
	if(((status & OEMMODE) == 0) && ((status & CDMODE) == 0))
	{
		if((status & VERBOSE) != 0) puts("No type specified, winkgen will generate both kinds of keys.");
		status |= OEMMODE;
		status |= CDMODE;
	}

	if((status & MULTI) != 0)
	{
		if((status & ANSI) != 0) printf("%sThis program will generate %d keys.%s\n", GREEN, times, RESET);
		else printf("This program will generate %d keys.\n", times);
	}

	if((status & OEMMODE) != 0) genOEM();
	if((status & CDMODE) != 0) genCD();

	exit(EXIT_SUCCESS);
}
