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
#define I2C_COM_CAL_COUNT   8

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

// Main FSM
typedef enum {
    M_INIT,             // Initialize the LCD
    M_SHOW_MEASURE1,    // Show measured V and I from I�C
    M_SHOW_MEASURE2,    // Show measured T and I_ from I�C
    M_SEL_VOLTAGE,      // Select the voltage
    M_CHA_VOLTAGE,      // Change the voltage
    M_SEL_CURRENT,      // Select the current
    M_CHA_CURRENT,      // Change the current
    M_SEL_CALIBRATION,  // Select calibration
    M_CHA_CALIBRATION,  // Change calibration
    M_SEL_SETTINGS,     // Select settings
    M_CHA_SETTINGS      // Change settings
            
} MainFSMState_e;

// Calibrate FSM
typedef enum {
    C_INIT,
    C_SET_DESIRED,
    C_CALIBRATE,
    C_SAVE,
    C_DONE,
    C_STOP,
    
    C_SEND_TO_SLAVE,
    C_WAIT_FOR_SLAVE
} CalibrateFSMState_e;

// Settings FSM
typedef enum {
    S_INIT,
    S_SEL_BRIGHTNESS,
    S_CHA_BRIGHTNESS,
    S_SEL_CONTRAST,
    S_CHA_CONTRAST,
    S_STOP
} SettingsFSMState_e;


typedef struct {
    uint16_t desiredVoltage;            /* Value it should be                 */
    uint16_t calibratedVoltage;         /* Value calibrated                   */
    uint16_t measuredCurrent;           /* Measured current                   */    
} Calibration_t;



typedef struct {
    MainFSMState_e currentState;        /* Current state of the FSM           */
    MainFSMState_e nextState;           /* Next calculated state              */
    MainFSMState_e saveState;           /* Saved state when leaving main FSM  */
    bool execute;                       /* FSM should execute                 */
    uint16_t waitCnt;                   /* Delay count                        */
    uint16_t prescale;                  /* Pre-scale counter                  */
} MainFSM_t;

typedef struct {
    CalibrateFSMState_e currentState;   /* Current state of FSM               */ 
    CalibrateFSMState_e nextState;      /* Next state of FSM                  */
    CalibrateFSMState_e saveState;      /* State SLAVE needs to ACK           */
    uint16_t calibrationCount;          /* Calibration sequence count         */
    uint16_t desiredVoltage;            /* Desired voltage                    */
    uint16_t calibratedVoltage;         /* Adjustment to desired              */
} CalibrationFSM_t;

typedef struct {
    SettingsFSMState_e currentState;    /* Current state of FSM               */
    SettingsFSMState_e nextState;       /* Next state of FSM                  */
    SettingsFSMState_e saveState;       /* Save state                         */
    uint16_t contrast;                  /* LCD contrast from 1 -> 50          */
    uint16_t brightness;                /* LCD brightness from 1 -> 8         */
} SettingsFSM_t;


#endif	/* XC_HEADER_TEMPLATE_H */

