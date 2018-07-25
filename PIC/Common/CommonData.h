#ifndef COMMON_DATA_H
#define	COMMON_DATA_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#define I2C_COM_SET_V       0
#define I2C_COM_SET_I       1
#define I2C_COM_MSR_V       2
#define I2C_COM_MSR_I       3
#define I2C_COM_MSR_T       4
#define I2C_COM_MSR_I_      5
#define I2C_COM_STATUS      6
#define I2C_COM_CAL_STATE   7
#define I2C_COM_CAL_COUNT 8

#define STAT_VOID           0
#define STAT_RUNNING        1
#define STAT_CALIBRATING    2

#define VOLTAGE_STEP        40                      /* Step size in [mV]      */
#define VOLTAGE_MAX         4000                    /* Maximum in [mV]        */
#define VOLTAGE_MIN         40                      /* Minimum in [mV]        */
#define CURRENT_STEP        40                      /* Step size in [mA]      */
#define CURRENT_MAX         2000                    /* Maximum in [mA]        */
#define CURRENT_MIN         0                       /* Minimum in [mA]        */

#define CALIB_MIN           400                     /* 1V                     */
#define CALIB_MAX           4000                    /* 10V                    */
#define CALIB_STEP          400                     /* Per 1V                 */
#define CALIB_mSTEP         4                       /* 10mV adjusting step    */ 


typedef union {
    struct {
        uint16_t statusCode       : 3;
        uint16_t errorCode        : 3;
        uint16_t currentClip      : 1;
        uint16_t outputEnabled    : 1;
        uint16_t pidEnabled       : 1;
        uint16_t calibrateEnabled : 1;
        uint16_t calibrationSt    : 6;
    };
    uint16_t value;
} SupplyStatus_t;

typedef struct {
    uint16_t desiredVoltage;        // Value it should be
    uint16_t calibratedVoltage;     // Value it needs to get the desired output
    uint16_t measuredCurrent;       // Measured current for the calibrated voltage    
} Calibration_t;

// Main FSM
typedef enum {
    M_INIT,         // Initialize the LCD
    M_SHOW_MEASURE, // Show measured data from I²C
    M_SEL_VOLTAGE,  // Select the voltage
    M_CHA_VOLTAGE,  // Change the voltage
    M_SEL_CURRENT,  // Select the current
    M_CHA_CURRENT,  // Change the current
    M_SEL_CALIBRATION, // Select calibration
    M_CHA_CALIBRATION, // Change calibration
    M_SEL_SETTINGS, // Select settings
    M_CHA_SETTINGS  // Change settings
            
} MainFSMState_e;

// Calibrate FSM
typedef enum {
    C_INIT = 1,
    C_SET_DESIRED,
    C_CALIBRATE,
    C_SAVE,
    C_DONE,
    C_STOP,
    
    C_SEND_TO_SLAVE,
    C_WAIT_FOR_SLAVE
} CalibrateFSMState_e;

typedef struct {
    CalibrateFSMState_e currentState;           /* Current state of FSM       */ 
    CalibrateFSMState_e nextState;              /* Next state of FSM          */
    CalibrateFSMState_e acknowledgeState;       /* State SLAVE needs to ACK   */
    uint16_t calibrationCount;                  /* Calibration sequence count */
    uint16_t desiredVoltage;                    /* Desired voltage            */
    uint16_t calibratedVoltage;                 /* Adjustment to desired      */
} CalibrationFSM_t;

#endif	/* XC_HEADER_TEMPLATE_H */

