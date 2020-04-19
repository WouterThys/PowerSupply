#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

#include "Utils.h"


bool stringEquals(const char * str1, const char * str2) {
    return (strcmp(str1, str2) == 0);
}

uint16_t stringToInt(const char * str) {
    uint16_t val = 0;
    uint16_t tmp = 0;
    uint16_t shift = 0;
    while(*str != '\0') {
        tmp = *str - 0x30;
        if (shift != 0) {
            val *= 10;
        }
        val += tmp;
        shift++;
        str++;
    }
    return val;
}

uint16_t minimumValue(float *values, uint16_t length) {
    float min = values[0];
    uint16_t ndx = 0;
    uint16_t i;
    for (i = 1; i < length; i++) {
        if (values[i] < min) {
            min = values[i];
            ndx = i;
        }
    }
    return ndx;
}

void concatinate(uint8_t msb, uint8_t lsb, uint16_t * out) {
    *out = (msb << 8) + lsb;
}

void split(uint16_t in, uint8_t * msb, uint8_t * lsb) {
    *msb = (uint8_t)(in >> 8);
    *lsb = (uint8_t)in;
}
