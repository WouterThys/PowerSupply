#ifndef ENC_DRIVER_H
#define	ENC_DRIVER_H

// ----------------------------------------------------------------------------

#define ENC_NORMAL        (1 << 1)   // use Peter Danneger's decoder
#define ENC_FLAKY         (1 << 2)   // use Table-based decoder

// ----------------------------------------------------------------------------

#ifndef ENC_DECODER
#  define ENC_DECODER     ENC_NORMAL
#endif

#if ENC_DECODER == ENC_FLAKY
#  ifndef ENC_HALFSTEP
#    define ENC_HALFSTEP  1        // use table for half step per default
#  endif
#endif

// ----------------------------------------------------------------------------

typedef enum {
    Open = 0,
    Closed,
    Pressed,
    Held,
    Released,
    Clicked,
    DoubleClicked
} Button_e;

/**
 * 
 */
void encDriverInit();

/**
 * 
 */
void encDriverService();

/**
 * 
 * @return 
 */
int16_t encDriverGetValue();

/**
 * 
 * @return 
 */
Button_e encDriverGetButton();



#endif	/* ENC_DRIVER_H */

