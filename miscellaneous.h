#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H

#include <stdarg.h>
#include <inttypes.h>

/*
 * endrev16: This function reverses the byte order,
 * namely endianness, for an uint16_t number.
 */
inline void endrev16(uint16_t *p) {
   *p = *p << 8 | *p >> 8;
}

/*
 * endrev32: This function reverses the byte order,
 * namely endianness, for an uint32_t number.
 */
inline void endrev32(uint32_t *p) {
   *p = *p << 24 | (*p & 0x0000FF00) << 8 |
        (*p & 0x00FF0000) >> 8 | (*p & 0xFF000000) >> 24;
}

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
 * count_digit: This function counts how many digits
 * the number consists of.
 */
int count_digit(uint32_t number);

/*
 * print_progress_bar: This function displays how much of the
 * work has been done.
 */
void print_progress_bar(
   uint32_t current, uint32_t total, int total_digit);

#endif