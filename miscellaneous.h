#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H

#include <stdarg.h>
#include <inttypes.h>

/*
 * raise_err: This funciton prints an error to the
 * stderr stream.
 */
void raise_err(char *, ...);

/*
 * get_endianness: This function checks which endianness
 * this machine follows. The return value 0 means big
 * endian, while 1 means little endian.
 */
int get_endianness(void);

/*
 * Note: These two functions reverse the byte
 * order, namely endianness.
 */
inline void endrev16(uint16_t *p) {
   *p = *p << 8 | *p >> 8;
}

inline void endrev32(uint32_t *p) {
   *p = *p << 24 | (*p & 0x0000FF00) << 8 |
        (*p & 0x00FF0000) >> 8 | (*p & 0xFF000000) >> 24;
}

#endif