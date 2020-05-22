#include "Bitmaps.h"

const unsigned char _check_false[CHECK_FALSE_SIZE] = {
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
const unsigned char _check_true[CHECK_TRUE_SIZE] = {
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

#define ARROW_ID   4
#define ARROW_SIZE 6
const unsigned char _arrow[ARROW_SIZE] = {
    6,  // Width
    5,  // Height
    0b00010000,
    0b00101111,
    0b11000010,
    0b00010000
};

#define NO_ARROW_ID   5
#define NO_ARROW_SIZE 6
const unsigned char _no_arrow[NO_ARROW_SIZE] = {
    6,  // Width
    5,  // Height
    0b00000000,
    0b00001100,
    0b00000000,
    0b00000000
};
