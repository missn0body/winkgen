#ifndef winkgen__winkgen__h
#define winkgen__winkgen__h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
	Main winkgen functions.

	Made by anson in 2024, see LICENSE for related details
*/

#include "include.h"

// Values representing individual bits to switch on or off in .flags
enum : char { VEBOSE = (1 << 1), QUIET = (1 << 2), NOLENERR = (1 << 3), DOUBLECHK = (1 << 4) };

static constexpr unsigned short TCDKEY = 11;
static constexpr unsigned short ECDKEY = 12;
static constexpr unsigned short OEMLEN = 21;

// Context structure for winkgen, holding variables for the 3 different keys
// that the program can generate. These keys are not allocated all at once
// and are only done so when they are used, otherwise they are null pointers
typedef struct winkgen_context_structure
{
	uint8_t flags;

	char *ten_cd_key;
	char *eleven_cd_key;
	char *oem_key;

} winkgen_ctx;

// These functions are declared before the generation functions because
// these functions are called by the later functions.
void verify_ten_cd_winkgen	(winkgen_ctx *);
void verify_eleven_cd_winkgen	(winkgen_ctx *);
void verify_oem_winkgen		(winkgen_ctx *);

void ten_cd_winkgen		(winkgen_ctx *);
void eleven_cd_winkgen		(winkgen_ctx *);
void oem_winkgen		(winkgen_ctx *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* winkgen__winkgen__h */
