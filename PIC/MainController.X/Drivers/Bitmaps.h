#ifndef BITMAPS_H_
#define	BITMAPS_H_

#include <xc.h> 
#include <stdint.h>

#define UPLOAD_BITMAPS   1  /* Set to '1' when bitmaps need to be written     */

#define CHECK_FALSE_ID   2
#define CHECK_FALSE_SIZE 10
const uint8_t _check_false[] = {
    8,  // Width
    8,  // Height
    0b00000000,
    0b11111110,
    0b10000010,
    0b10000010,
    0b10000010,
    0b10000010,
    0b10000010,
    0b11111110
};

#define CHECK_TRUE_ID   3
#define CHECK_TRUE_SIZE 10
const uint8_t _check_true[] = {
    8,  // Width
    8,  // Height
    0b00000000,
    0b11111110,
    0b10000110,
    0b10001110,
    0b11011010,
    0b11110010,
    0b10100010,
    0b11111110
};


#endif	

