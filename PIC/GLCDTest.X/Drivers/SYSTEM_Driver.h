/* 
 * File:   System functions
 * Author: wouter
 *
 * Created on 3 maart 2015, 13:06
 */

#ifndef SYSTEM_DRIVER_H
#define	SYSTEM_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <libpic30.h>
#include <stdint.h>
#include <stdbool.h>

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        36850000L
#define FCY             SYS_FREQ/2

#define DelayMs(d) \
  { __delay32( (unsigned long) (((unsigned long long) d)*(FCY)/1000ULL)); }
#define DelayUs(d) \
  { __delay32( (unsigned long) (((unsigned long long) d)*(FCY)/1000000ULL)); }
    
    
#define TRAP_OSC    -100
#define TRAP_MATH   -101
#define TRAP_ADDR   -102
#define TRAP_DMA    -103
#define TRAP_STACK  -104

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
    
/**
 * Configure oscillator settings.
 */
void sysInitOscillator(void); 

/**
 * Configure PLL settings.
 */
void sysInitPll(void); 

/**
 * Initialize the ports all as output, clear analog selections.
 */
void sysInitPorts(void);

/**
 * Initializes the interrupts to the default settings.
 */
void sysInitInterrupts(void);

/**
 * Enable all interrupts
 * @param enable
 */
void sysInterruptEnable(bool enable);

/**
 * 
 * @param onError
 */
void sysInitError(void (*onError)(int16_t code));

/**
 * Reset system after defined delay
 * @param afterDelay
 */
void sysReset(uint16_t afterDelay);


#endif