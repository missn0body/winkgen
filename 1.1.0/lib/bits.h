#ifndef anson__bits__h
#define anson__bits__h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
	Made by anson in 2024, see LICENSE for related details
*/

#include <stdint.h>

inline void   set(uint8_t *in, char what) { (*in) |=   what;  }
inline void unset(uint8_t *in, char what) { (*in) &= (~what); }
inline bool  test(uint8_t *in, char what) { return (((*in) & what) != 0); }

inline const char *boolstr(const bool what) 	   { return (what) ? "true" : "false"; }
inline const char  *bitstr(uint8_t *in, char what) { return boolstr(test(in, what));   }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* anson__bits__h */
