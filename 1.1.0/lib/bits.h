#ifndef winkgen__bits__h
#define winkgen__bits__h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
	General use bitshifting functions, originally made for winkgen

	Made by anson in 2024, see LICENSE for related details
*/

static inline void   set(uint8_t *in, char what) { (*in) |=   what;  }
static inline void unset(uint8_t *in, char what) { (*in) &= (~what); }
static inline bool  test(uint8_t *in, char what) { return (((*in) & what) != 0); }

static inline const char *boolstr(const bool what) 	  { return (what) ? "true" : "false"; }
static inline const char  *bitstr(uint8_t *in, char what) { return boolstr(test(in, what));   }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* winkgen__bits__h */
