#ifndef COMMON_DATA_H
#define	COMMON_DATA_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#define I2C_COM_SET_V   0
#define I2C_COM_SET_I   1
#define I2C_COM_MSR_V   2
#define I2C_COM_MSR_I   3
#define I2C_COM_MSR_T   4
#define I2C_COM_MSR_I_  5
#define I2C_COM_STATUS  6

#define STAT_VOID       0
#define STAT_RUNNING    1

typedef union {
    struct {
        uint16_t statusCode     : 4;
        uint16_t errorCode      : 4;
        uint16_t outputEnabled  : 1;
        uint16_t currentClip    : 1;
        uint16_t                : 6;
    };
    uint16_t value;
} SupplyStatus_t;


#endif	/* XC_HEADER_TEMPLATE_H */

