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

typedef enum Button_e {
    Open = 0,
    Closed,
    Pressed,
    Held,
    Released,
    Clicked,
    DoubleClicked
} Button_t;


void encInit();
void encService();
int16_t encGetValue();
Button_t encGetButton();



#endif	/* ENC_DRIVER_H */

