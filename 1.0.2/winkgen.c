// Lets generate 90's-era licensing keys!
// https://gurney.dev/posts/mod7/
// https://github.com/missn0body/winkgen

// 5/28/20 - Yay! generating W*****s CD keys works! I've ironed out most of the possible bugs,
//           except for one where the program can not generate zeros in the keys, due to the way
//           I was converting my digit arrays into full integers, long story short, 0 * 1000 is
//           still zero, so there are no zeros appearing in the generated license keys. Going
//           to work on OEM keys tomorrow, I'm sleepy.

// 6/3/20 - Yay again! I've finished OEM keys and made all site generations their own function
//          so it could be called multiple times in the case the user wants more than one
//          generated. Ironed out all bugs, I'll start implementing command-line arguments
//          and bringing it all together in the main() function. It's been nice.

// 9/19/23 - Began the rewrite of this program, since I really wanted to add command-line arguments
//	     in the original program but for some reason didn't. I also didn't want to put
//	     getopt_long() in this program so I managed to write it on my own argument parser.
//	     On this date I first pushed this project via Git to my new Github, which I have
//	     never used before.

// 9/20/23 - Finally finished the OEM key generating section and I seemed to have ironed out
//	     all of the bugs, but please tell me if there still are some.

// 9/21/23 - Fixed a bug where the third section of the OEM key generating section wasn't
//	     actually summing up digits at all.
// 2.
//	     Finished the CD key generating section and it seems to work alright, I'm sure people
//	     will let me know if there are. There doesn't seem to be much else to add to this
//	     program, and if there is, I'll add it in another version later.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h> // used for sleep()

// So, a primitive is composed of bits, 8 bits
// to a byte, and a character is (usually) guarenteed
// to be a single byte. A bit can either be on or off.
// Which means, with a single character, you've got
// a set of 8 booleans to use as flags, which I think
// is really really neat.

#define VERBOSE		(1 << 1)
#define OEMMODE		(1 << 2)
#define CDMODE		(1 << 3)
#define ANSI		(1 << 4)

// ANSI escape codes, to use for color output, cause
// I quite like colors in my terminal :)
#define RED		"\033[30;101m"
#define BLUE    	"\033[34m"
#define GREEN   	"\033[32m"
#define RESET		"\033[0m"

const char *VERSION = "v. 1.0.2.";

// We're not using "char", just in case there's some
// dystopian future where they changed the definition
// of "char". Might be just bias, but I put more trust
// in stdint.h's typedefs.
typedef uint8_t flag_t;
static flag_t status = 0;

const char *errorMes[] = { "too few arguments", "unknown option" };
// Quick mnemonics for indexing the above array.
enum { ARG = 0, UNK};

// This function is not meant to be called directly, unless you
// want to provide a file and line number directly instead of
// nifty GCC extensions.
void _error(char *file, int line, int index, flag_t *status)
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

#define greenIfColor(x)	((((x) & ANSI) != 0) ? GREEN : "")
#define redIfColor(x)	((((x) & ANSI) != 0) ? RED : "")
#define blueIfColor(x)	((((x) & ANSI) != 0) ? BLUE : "")
#define resetIfColor(x)	((((x) & ANSI) != 0) ? RESET : "")

#define alert(x, ...)	fprintf(stderr, "%s%s%s\n", redIfColor(x), __VA_ARGS__, resetIfColor(x))

static inline unsigned int randomInt(unsigned int minimum, unsigned int maximum)
{
	return (unsigned int)(rand() % ((maximum - minimum) + 1)) + minimum;
}

void genOEM(flag_t *input)
{
	if((*input & VERBOSE) != 0)
                printf("%sGenerating OEM key...%s\n", greenIfColor(*input), resetIfColor(*input));

	// Plus one for null terminator
	char returnValue[24];
	char first[6], fourth[6];

	// First three numbers is a day, between one and 366.
	for(int i = 0; i < 3; i++) first[i] = randomInt((i >= 2) ? 1 : 0, (i >= 2) ? 6 : 3) + '0';

	// Second two numbers are a year, from 95 to 03.
	first[3] = (randomInt(0, 1) == 1) ? ('9') : ('0');
	if(first[3] == '9') 	 first[4] = randomInt(5, 9) + '0';
	else if(first[3] == '0') first[4] = randomInt(0, 3) + '0';

	// Add null terminator.
	first[5] = '\0';
	// Verbose (debug) print.
	if((*input & VERBOSE) != 0)
		printf("%sGenerated first OEM section \"%s\"...%s\n", blueIfColor(*input), first, resetIfColor(*input));

	// The second section is really simple. But we'll save
	// it for later.

	// The digits of the third section must be divisible
	// by 7, and the last digit must not be 0 or greater than 7
	unsigned int test = 0, sum = 0, holder = 0;
	for(int tmi = 0; ; tmi++)
	{
		test = randomInt(111111, 999996);
		// Check for digits in order to break.
		if((test % 10) <= 7 && (test % 10) >= 1)
		{
			holder = test;
			// Iterate through the digits and
			// sum them up...
			while(holder != 0) { sum += (holder % 10); holder /= 10; }
			// ... and then see if it's divisible
			// by seven.
			if(sum % 7 == 0) break;
		}

		if(tmi == 10 && (*input & VERBOSE) != 0)
		{
			// Seriously, it is. You don't want to have your entire screen filled to the brim.
			alert(*input, "Verbose printing has been halted, this is for your own good.");
		}
		else if(tmi <= 9 && (*input & VERBOSE) != 0)
			printf("%sTesting third OEM section \"%u\"...%s\n", greenIfColor(*input), test, resetIfColor(*input));

		// The PRNG is seeded by the time, so wait until we get a new seed.
		sleep(1);
	}

	// Another debug print.
	if((*input & VERBOSE) != 0)
		printf("%sGenerated third OEM section \"%u\"...%s\n", blueIfColor(*input), test, resetIfColor(*input));

	// The fourth section is just random numbers.
	for(int i = 0; i < 6; i++) fourth[i] = randomInt(0, 9) + '0';
	fourth[5] = '\0';

	if((*input & VERBOSE) != 0)
		printf("%sGenerated fourth OEM section \"%s\"...%s\n", blueIfColor(*input), fourth, resetIfColor(*input));

	// Assemble the final key
	snprintf(returnValue, sizeof(returnValue), "%s-OEM-0%u-%s", first, test, fourth);
	printf("%s\n", returnValue);
	return;
}

void genCD(flag_t *input)
{
	if((*input & VERBOSE) != 0)
                printf("%sGenerating CD key...%s\n", greenIfColor(*input), resetIfColor(*input));

	// Plus one for null terminator.
	char returnValue[12], first[4];
	for(int tmi = 0; ; tmi++)
	{
		first[0] = randomInt(0, 9) + '0';
		first[1] = randomInt(0, 9) + '0';
		first[2] = randomInt(0, 8) + '0';
		if(first[3] != '\0') first[3] = '\0';
		// Really what is checked is if the first
		// section is 333, 444, 555, 666, 777, 888,
		// or 999, but I'm gonna ban all repeating
		// numbers just to be safe.
		if(first[0] != first[1] &&
		   first[0] != first[2] &&
		   first[1] != first[2])
			break;

		// Alert the user what is happening if they
		// ask for it.
		if(tmi == 10 && (*input & VERBOSE) != 0)
		{
			// Again, you don't want your screen filled up with numbers.
			alert(*input, "Verbose printing has been halted, this is for your own good.");
		}
		else if(tmi <= 9 && (*input & VERBOSE) != 0)
                        printf("%sTesting first CD section \"%s\"...%s\n", greenIfColor(*input), first, resetIfColor(*input));

		// The PRNG is seeded by the time, so wait until we get a new seed.
                sleep(1);
	}

	first[3] = '\0'; // Just to be safe.
	if((*input & VERBOSE) != 0)
                printf("%sGenerated first CD section \"%s\"...%s\n", blueIfColor(*input), first, resetIfColor(*input));

	// The second section of the CD key is where the mod7
	// algorithm takes place. Much like the previous function,
	// the digits added up must be divisible by 7, and the last
	// digit can not be 0 or greater than 7.

	unsigned int second = 0, sum = 0, holder = 0;
	for(int tmi = 0; ; tmi++)
	{
		second = randomInt(1111111, 9999997);
		if((second % 10) >= 1 && (second % 10) <= 7)
		{
			// Sum up all the digits.
			holder = second;
			while(holder != 0) { sum += (holder % 10); holder /= 10; }
			// If it's divisible by 7, hurray!
			if((sum % 7) == 0) break;
		}

		// Alert the user what is happening if they
                // ask for it.
                if(tmi == 10 && (*input & VERBOSE) != 0)
                {
                        // Again, you don't want your screen filled up with numbers.
                        alert(*input, "Verbose printing has been halted, this is for your own good.");
                }
                else if(tmi <= 9 && (*input & VERBOSE) != 0)
                        printf("%sTesting second CD section \"%u\"...%s\n", greenIfColor(*input), second, resetIfColor(*input));

		// The PRNG is seeded by the time, so wait until we get a new seed.
                sleep(1);
	}

	if((*input & VERBOSE) != 0)
                printf("%sGenerated second CD section \"%u\"...%s\n", blueIfColor(*input), second, resetIfColor(*input));

	snprintf(returnValue, sizeof(returnValue), "%s-%u", first, second);
	printf("%s\n", returnValue);
	return;
}

//////////////////////////////////////////////////////////////
// IMPORTANT FUNCTIONS END HERE
//////////////////////////////////////////////////////////////

void showUsage()
{
	printf("winkgen (%s): a sly key generator by anson ;)\n", VERSION);
        puts("Usage: winkgen -v (--verbose) -a (--ansi) -o (--oem) -c (--cd) -h (--help)\n");
        puts("Reminder, winkgen will not operate without command-line arguments,");
        puts("so use winkgen -o or winkgen -c to start generating keys.");
	return;
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	if(argc < 2) { error(ARG); exit(EXIT_FAILURE); }
	int c;

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
			if(strcmp((*argv) + 2, "help") == 0)    { showUsage(); exit(EXIT_SUCCESS); }
			if(strcmp((*argv) + 2, "verbose") == 0) { status |= VERBOSE; continue; }
			if(strcmp((*argv) + 2, "oem") 	  == 0) { status |= OEMMODE; continue; }
			if(strcmp((*argv) + 2, "cd") 	  == 0) { status |= CDMODE; continue; }
			if(strcmp((*argv) + 2, "ansi") 	  == 0) { status |= ANSI; continue; }
		}
		while((c = *++argv[0]))
		{
			// Single character option testing here.
			switch(c)
			{
				case 'h': showUsage(); exit(EXIT_SUCCESS);
				case 'v': status |= VERBOSE; break;
				case 'o': status |= OEMMODE; break;
				case 'c': status |= CDMODE;  break;
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
		printf("%sVerbose flag is set.%s\n", greenIfColor(status), resetIfColor(status));
	}

	// If you don't specify whether you want an OEM key or CD key,
	// Then you get both.
	if(((status & OEMMODE) == 0) && ((status & CDMODE) == 0))
	{
		if((status & VERBOSE) != 0) puts("No type specified, winkgen will generate both kinds of keys.");
		status |= OEMMODE;
		status |= CDMODE;
	}

	puts("You may need to wait a several seconds or so to generate a key,\nplease be patient...");
	if((status & OEMMODE) != 0) genOEM(&status);
	if((status & CDMODE) != 0) genCD(&status);

	exit(EXIT_SUCCESS);
}
