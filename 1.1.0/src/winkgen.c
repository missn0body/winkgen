// Lets generate 90's-era licensing keys!
// https://gurney.dev/posts/mod7/

// C standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// winkgen and other libraries
// These header files have a lot
// of the same standard libraries
// as the above in case of reuse
#include "../lib/winkgen.h"
#include "../lib/bits.h"

const char *VERSION = "v.1.1.0";

// Make the context global, that way on exit,
// we can safely free the contents as well as
// the structure itself
static winkgen_ctx *ctx = nullptr;

// This function will be called on exit to safely
// free allocated memory without worry
static void free_ctx(void)
{
	if(ctx->ten_cd_key    != nullptr) free(ctx->ten_cd_key);
	if(ctx->eleven_cd_key != nullptr) free(ctx->eleven_cd_key);
	if(ctx->oem_key       != nullptr) free(ctx->oem_key);
	free(ctx);
}

// Usage function
static void usage(void)
{
	printf("winkgen (%s): a sly little keygen, *wink*\n", VERSION);
	printf("created by anson <thesearethethingswesaw@gmail.com>\n\n");
	printf("usage:\n");
	printf("  winkgen -h | --help\n");
	printf("  winkgen -g | --gen KEYTYPE [--verbose | --quiet]\n");
	printf("  winkgen -v | --ver KEYTYPE <KEY> [--verbose | --quiet]\n\n");
	// Descrbing modes
	printf("options:\n");
	printf("%13s\t%s\n",   "-g, --gen", "generates a key");
	printf("%13s\t%s\n\n", "-v, --ver", "verifies a key");
	// Describing key types
	printf("%13s\t%s\n",   "--oem",  "specifies an OEM key");
	printf("%13s\t%s\n",   "--cd10", "specifies an 10-digit CD key");
	printf("%13s\t%s\n\n", "--cd11", "specifies an 11-digit CD key");
	// Describing other flags
	printf("%13s\t%s\n",   "--verbose",   "prints additional diagnostic information");
	printf("%13s\t%s\n\n", "-q, --quiet", "silents output (no writes to stdout)");
	printf("copyright (c) 2024, see LICENSE for related details\n");
	return;
}

int main(int argc, char *argv[])
{
	// Some minimal setup in case of early exit
	srand(time(NULL));
	char *programname = argv[0];

	// Check if we have enough arguments
	if(argc < 2)
	{
		fprintf(stderr, "%s: too few arguments, try \"--help\"\n", programname);
		return EXIT_FAILURE;
	}

	// Declaring the context
	ctx = malloc(sizeof(winkgen_ctx));
	if(ctx == NULL) { perror(programname); return EXIT_FAILURE; }

	// Loading the structure with default values
	ctx->flags 	   = 0x00;
	ctx->ten_cd_key    = nullptr;
	ctx->eleven_cd_key = nullptr;
	ctx->oem_key 	   = nullptr;

	atexit(free_ctx);

	int c;
	bool gen_or_verify  = true; // true for generate, false for verify
	short keytype       = -1;   // 0 for OEM, 1 for 10-digit CD, 2 for 11-digit CD
	char keyarg[OEMLEN] = {0};

	while(--argc > 0 && (*++argv)[0] != '\0')
	{
		// Non option argument, let's assume its <KEY>
		if((*argv)[0] != '-')
		{
			if(keyarg[0] != '\0')
			{
				printf("%s: discarded program input -- \"%s\"\n", programname, *argv);
				continue;
			}

			strncpy(keyarg, *argv, (OEMLEN - 1));
		}
		// If there's another dash, then it's a long option.
		// Move the pointer up 2 places and compare the word itself.
		if((*argv)[0] == '-')
		{
			if((*argv)[1] == '-')
			{
				// Using continue statements here so that the user
				// can use both single character and long options
				// simultaniously, and the loop can test both.

				// Meta operations
				if(strcmp((*argv) + 2, "help") 	  == 0) { usage(); return EXIT_SUCCESS;}
				if(strcmp((*argv) + 2, "verbose") == 0) { set(&ctx->flags, VERBOSE); continue; }
				if(strcmp((*argv) + 2, "quiet")   == 0) { set(&ctx->flags, QUIET);   continue; }
				// Modes of operations
				if(strcmp((*argv) + 2, "gen") 	  == 0) { gen_or_verify = true;  continue; }
				if(strcmp((*argv) + 2, "ver")     == 0) { gen_or_verify = false; continue; }
				// Types of keys
				if(strcmp((*argv) + 2, "oem") 	  == 0) { keytype = 0; continue; }
				if(strcmp((*argv) + 2, "cd10") 	  == 0) { keytype = 1; continue; }
				if(strcmp((*argv) + 2, "cd11") 	  == 0) { keytype = 2; continue; }
			}
			while((c = *++argv[0]))
			{
				// Single character option testing here.
				switch(c)
				{
					// No verbose for the meta operations, I would
					// rather use 'v' for 'verify'
					case 'h': usage(); return EXIT_SUCCESS;
					case 'q': set(&ctx->flags, QUIET); break;

					// There are no single character options for
					// the types of keys, as I wouldn't know how to
					// properly abbreviate it.
					case 'g': gen_or_verify = true;  break;
					case 'v': gen_or_verify = false; break;

					// This error flag can either be set by a
					// completely unrelated character inputted,
					// or you managed to put -option instead of
					// --option.
					default :
						fprintf(stderr, "%s: incorrect option -- \"%s\"\n", programname, (*argv + 1));
						return EXIT_FAILURE;
				}
			}
		}
	}

	// Catch if the user never specified a key type
	if(keytype == -1)
	{
		fprintf(stderr, "%s: keytype not chosen, exiting...\n", programname);
		return EXIT_FAILURE;
	}

	// If we're verifying a key, but haven't gotten one, catch and exit
	if(gen_or_verify == false && keyarg[0] == '\0')
	{
		fprintf(stderr, "%s: key input not specified, exiting...\n", programname);
		return EXIT_FAILURE;
	}

	// Verbose printing before any operations
	if(test(&ctx->flags, VERBOSE))
	{
		printf("%s: verbose mode activated\n", programname);
		printf("%s: current mode is \"%s\"\n", programname, (gen_or_verify) ? "generate" : "verify");
		switch(keytype)
		{
			case 0:  printf("%s: keytype is OEM (%d)\n", programname, keytype); break;
			case 1:  printf("%s: keytype is CD10 (%d)\n", programname, keytype); break;
			case 2:  printf("%s: keytype is CD11 (%d)\n", programname, keytype); break;
			default: printf("%s: unknown keytype (%d), fault\n", programname, keytype); break;
		}

		if(keyarg[0] != '\0')
			printf("%s: program input captured \"%s\", treating as key\n", programname, keyarg);
	}

	return EXIT_SUCCESS;
}
