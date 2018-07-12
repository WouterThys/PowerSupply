#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>        /* Includes string functions                       */

#include "../Settings.h"
#include "SYSTEM_Driver.h"
#include "../Controllers/MCP_Controller.h"
#include "ENC_Driver.h"

// ----------------------------------------------------------------------------
// Button configuration (values for 1ms timer service calls)
//
#define ENC_BUTTONINTERVAL    10  // check button every x milliseconds, also debouce time
#define ENC_DOUBLECLICKTIME  600  // second click within 600ms
#define ENC_HOLDTIME        1200  // report held button after 1.2s

// ----------------------------------------------------------------------------
// Acceleration configuration (for 1000Hz calls to ::service())
//
#define ENC_ACCEL_TOP      3072   // max. acceleration: *12 (val >> 8)
#define ENC_ACCEL_INC        25
#define ENC_ACCEL_DEC         2

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#if ENC_DECODER != ENC_NORMAL
#  ifdef ENC_HALFSTEP
     // decoding table for hardware with flaky notch (half resolution)
     const int8_t encoderTable[16] = { 
       0, 0, -1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 0, 0 
     };    
#  else
     // decoding table for normal hardware
     const int8_t encoderTable[16] = { 
       0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 
     };    
#  endif
#endif
     
static Button_e button;     
static int16_t delta;
static int16_t last;
static uint8_t steps = 4;
static uint16_t acceleration;

static uint16_t keyDownTicks = 0;
static uint8_t doubleClickTicks = 0;
static unsigned long lastButtonCheck = 0;
static unsigned long now = 0;

// ----------------------------------------------------------------------------
     
void encDriverInit() {
    
    mcpInit();
    
    uint8_t read = mcpGetPORTB();
    
    if ((read & 0x80) == 1) last = 3;
    if ((read & 0x40) == 1) last ^= 1;
    
}     

// ----------------------------------------------------------------------------
// call this every 1 millisecond via timer ISR
//
void encDriverService(){
    bool moved = false;
    now++;

    acceleration -= ENC_ACCEL_DEC;
    if (acceleration & 0x8000) { // handle overflow of MSB is set
      acceleration = 0;
    }
  
    uint8_t read = mcpGetPORTB();

#if ENC_DECODER == ENC_FLAKY
  last = (last << 2) & 0x0F;

  if ((read & 0x80) > 0) {
    last |= 2;
  }

  if ((read & 0x40) > 0) {
    last |= 1;
  }

  uint8_t tbl = encoderTable[last]; 
  if (tbl) {
    delta += tbl;
    moved = true;
  }
#elif ENC_DECODER == ENC_NORMAL
  int8_t curr = 0;

  if ((read & 0x80) > 0) {
    curr = 3;
  }

  if ((read & 0x40) > 0) {
    curr ^= 1;
  }
  
  int8_t diff = last - curr;

  if (diff & 1) {            // bit 0 = step
    last = curr;
    delta += (diff & 2) - 1; // bit 1 = direction (+/-)
    moved = true;    
  }
#else
# error "Error: define ENC_DECODER to ENC_NORMAL or ENC_FLAKY"
#endif

  if (moved) {
    // increment accelerator if encoder has been moved
    if (acceleration <= (ENC_ACCEL_TOP - ENC_ACCEL_INC)) {
      acceleration += ENC_ACCEL_INC;
    }
  }

  // handle button
  //
#ifndef WITHOUT_BUTTON
  

  // checking button is sufficient every 10-30ms
  if ((now - lastButtonCheck) >= ENC_BUTTONINTERVAL) { 
    lastButtonCheck = now;
    
    if ((read & 0x20) == 0) { // key is down
      keyDownTicks++;
      if (keyDownTicks > (ENC_HOLDTIME / ENC_BUTTONINTERVAL)) {
        button = Held;
      }
    }

    if ((read & 0x20) > 0) { // key is now up
      if (keyDownTicks /*> ENC_BUTTONINTERVAL*/) {
        if (button == Held) {
          button = Released;
          doubleClickTicks = 0;
        }
        else {
          #define ENC_SINGLECLICKONLY 1
          if (doubleClickTicks > ENC_SINGLECLICKONLY) {   // prevent trigger in single click mode
            if (doubleClickTicks < (ENC_DOUBLECLICKTIME / ENC_BUTTONINTERVAL)) {
              button = DoubleClicked;
              doubleClickTicks = 0;
            }
          }
          else {
            doubleClickTicks = (ENC_DOUBLECLICKTIME / ENC_BUTTONINTERVAL);
          }
        }
      }

      keyDownTicks = 0;
    }
  
    if (doubleClickTicks > 0) {
      doubleClickTicks--;
      if (--doubleClickTicks == 0) {
        button = Clicked;
      }
    }
  }
#endif // WITHOUT_BUTTON

}

// ----------------------------------------------------------------------------

int16_t encDriverGetValue(void){
  int16_t val;
  
  val = delta;

  if (steps == 2) delta = val & 1;
  else if (steps == 4) delta = val & 3;
  else delta = 0; // default to 1 step per notch

  
  if (steps == 4) val >>= 2;
  if (steps == 2) val >>= 1;

  int16_t r = 0;
  int16_t accel = (acceleration >> 8);

  if (val < 0) {
    r -= 1 + accel;
  }
  else if (val > 0) {
    r += 1 + accel;
  }

  return r;
}

// ----------------------------------------------------------------------------

#ifndef WITHOUT_BUTTON
Button_e encDriverGetButton(void) {
  Button_e ret = button;
  if (button != Held) {
    button = Open; // reset
  }
  return ret;
}
#endif