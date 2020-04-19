#ifndef MCP_MODULE_H
#define	MCP_MODULE_H



#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

    
/******************************************************************************/
/* Register values                                                            */
/******************************************************************************/


// !!! NOTE !!! 
// IOCON.BANK should be 0
// !!! NOTE !!!

/************************* IODIRA *********************************************/   
struct IODIRA_s {
    union {
        struct {
            unsigned IO0:1;         
            unsigned IO1:1;         
            unsigned IO2:1;         
            unsigned IO3:1;         
            unsigned IO4:1;         
            unsigned IO5:1;         
            unsigned IO6:1;         
            unsigned IO7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct IODIRA_s IODIRABITS;
volatile IODIRABITS mcpIODIRAbits = {{.value = 0xFF}, .address = 0x00};
#define mcpIODIRA   mcpIODIRAbits.value


/************************* IODIRB *********************************************/   
struct IODIRB_s {
    union {
        struct {
            unsigned IO0:1;         
            unsigned IO1:1;         
            unsigned IO2:1;         
            unsigned IO3:1;         
            unsigned IO4:1;         
            unsigned IO5:1;         
            unsigned IO6:1;         
            unsigned IO7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct IODIRB_s IODIRBBITS;
volatile IODIRBBITS mcpIODIRBbits = {{.value = 0xFF}, .address = 0x01};
#define mcpIODIRB   mcpIODIRBbits.value



/************************* IOPOLA *********************************************/   
struct IOPOLA_s {
    union {
        struct {
            unsigned IP0:1;         
            unsigned IP1:1;         
            unsigned IP2:1;         
            unsigned IP3:1;         
            unsigned IP4:1;         
            unsigned IP5:1;         
            unsigned IP6:1;         
            unsigned IP7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct IOPOLA_s IOPOLABITS;
volatile IOPOLABITS mcpIOPOLAbits = {{.value = 0x00}, .address = 0x02};
#define mcpIOPOLA   mcpIOPOLAbits.value



/************************* IOPOLB *********************************************/   
struct IOPOLB_s {
    union {
        struct {
            unsigned IP0:1;         
            unsigned IP1:1;         
            unsigned IP2:1;         
            unsigned IP3:1;         
            unsigned IP4:1;         
            unsigned IP5:1;         
            unsigned IP6:1;         
            unsigned IP7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct IOPOLB_s IOPOLBBITS;
volatile IOPOLBBITS mcpIOPOLBbits = {{.value = 0x00}, .address = 0x03};
#define mcpIOPOLB   mcpIOPOLBbits.value



/************************* GPINTENA *******************************************/   
struct GPINTENA_s {
    union {
        struct {
            unsigned GPINT0:1;         
            unsigned GPINT1:1;         
            unsigned GPINT2:1;         
            unsigned GPINT3:1;         
            unsigned GPINT4:1;         
            unsigned GPINT5:1;         
            unsigned GPINT6:1;         
            unsigned GPINT7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct GPINTENA_s GPINTENABITS;
volatile GPINTENABITS mcpGPINTENAbits = {{.value = 0x00}, .address = 0x04};
#define mcpGPINTENA   mcpGPINTENAbits.value



/************************* GPINTENB *******************************************/   
struct GPINTENB_s {
    union {
        struct {
            unsigned GPINT0:1;         
            unsigned GPINT1:1;         
            unsigned GPINT2:1;         
            unsigned GPINT3:1;         
            unsigned GPINT4:1;         
            unsigned GPINT5:1;         
            unsigned GPINT6:1;         
            unsigned GPINT7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct GPINTENB_s GPINTENBBITS;
volatile GPINTENBBITS mcpGPINTENBbits = {{.value = 0x00}, .address = 0x05};
#define mcpGPINTENB   mcpGPINTENBbits.value


/************************* DEFVALA ********************************************/   
struct DEFVALA_s {
    union {
        struct {
            unsigned DEF0:1;         
            unsigned DEF1:1;         
            unsigned DEF2:1;         
            unsigned DEF3:1;         
            unsigned DEF4:1;         
            unsigned DEF5:1;         
            unsigned DEF6:1;         
            unsigned DEF7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct DEFVALA_s DEFVALABITS;
volatile DEFVALABITS mcpDEFVALAbits = {{.value = 0x00}, .address = 0x06};
#define mcpDEFVALA   mcpDEFVALAbits.value


/************************* DEFVALB ********************************************/   
struct DEFVALB_s {
    union {
        struct {
            unsigned DEF0:1;         
            unsigned DEF1:1;         
            unsigned DEF2:1;         
            unsigned DEF3:1;         
            unsigned DEF4:1;         
            unsigned DEF5:1;         
            unsigned DEF6:1;         
            unsigned DEF7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct DEFVALB_s DEFVALBBITS;
volatile DEFVALBBITS mcpDEFVALBbits = {{.value = 0x00}, .address = 0x07};
#define mcpDEFVALB   mcpDEFVALBbits.value



/************************* INTCONA ********************************************/   
struct INTCONA_s {
    union {
        struct {
            unsigned IOC0:1;         
            unsigned IOC1:1;         
            unsigned IOC2:1;         
            unsigned IOC3:1;         
            unsigned IOC4:1;         
            unsigned IOC5:1;         
            unsigned IOC6:1;         
            unsigned IOC7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct INTCONA_s INTCONABITS;
volatile INTCONABITS mcpINTCONAbits = {{.value = 0x00}, .address = 0x08};
#define mcpINTCONA   mcpINTCONAbits.value


/************************* INTCONB ********************************************/   
struct INTCONB_s {
    union {
        struct {
            unsigned IOC0:1;         
            unsigned IOC1:1;         
            unsigned IOC2:1;         
            unsigned IOC3:1;         
            unsigned IOC4:1;         
            unsigned IOC5:1;         
            unsigned IOC6:1;         
            unsigned IOC7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct INTCONB_s INTCONBBITS;
volatile INTCONBBITS mcpINTCONBbits = {{.value = 0x00}, .address = 0x09};
#define mcpINTCONB   mcpINTCONBbits.value



/************************* IOCON *********************************************/   
struct IOCON_s {
    union {
        struct {
            unsigned :1;         
            unsigned INTPOL:1;         
            unsigned ODR:1;         
            unsigned HAEN:1;         
            unsigned DISSLW:1;         
            unsigned SEQOP:1;         
            unsigned MIRROR:1;         
            unsigned BANK:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct IOCON_s IOCONBITS;
volatile IOCONBITS mcpIOCONbits = {{.value = 0x00}, .address = 0x0A};
#define mcpIOCON   mcpIOCONbits.value


/************************* GPPUA **********************************************/   
struct GPPUA_s {
    union {
        struct {
            unsigned PU0:1;         
            unsigned PU1:1;         
            unsigned PU2:1;         
            unsigned PU3:1;         
            unsigned PU4:1;         
            unsigned PU5:1;         
            unsigned PU6:1;         
            unsigned PU7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct GPPUA_s GPPUABITS;
volatile GPPUABITS mcpGPPUAbits = {{.value = 0x00}, .address = 0x0C};
#define mcpGPPUA   mcpGPPUAbits.value


/************************* GPPUB **********************************************/   
struct GPPUB_s {
    union {
        struct {
            unsigned PU0:1;         
            unsigned PU1:1;         
            unsigned PU2:1;         
            unsigned PU3:1;         
            unsigned PU4:1;         
            unsigned PU5:1;         
            unsigned PU6:1;         
            unsigned PU7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct GPPUB_s GPPUBBITS;
volatile GPPUBBITS mcpGPPUBbits = {{.value = 0x00}, .address = 0x0D};
#define mcpGPPUB   mcpGPPUBbits.value



/************************* INTFA **********************************************/   
struct INTFA_s {
    union {
        struct {
            unsigned INT0:1;         
            unsigned INT1:1;         
            unsigned INT2:1;         
            unsigned INT3:1;         
            unsigned INT4:1;         
            unsigned INT5:1;         
            unsigned INT6:1;         
            unsigned INT7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct INTFA_s INTFABITS;
volatile INTFABITS mcpINTFAbits = {{.value = 0x00}, .address = 0x0E};
#define mcpINTFA   mcpINTFAbits.value


/************************* INTFB **********************************************/   
struct INTFB_s {
    union {
        struct {
            unsigned INT0:1;         
            unsigned INT1:1;         
            unsigned INT2:1;         
            unsigned INT3:1;         
            unsigned INT4:1;         
            unsigned INT5:1;         
            unsigned INT6:1;         
            unsigned INT7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct INTFB_s INTFBBITS;
volatile INTFBBITS mcpINTFBbits = {{.value = 0x00}, .address = 0x0F};
#define mcpINTFB   mcpINTFBbits.value



/************************* INTCAPA ********************************************/   
struct INTCAPA_s {
    union {
        struct {
            unsigned ICP0:1;         
            unsigned ICP1:1;         
            unsigned ICP2:1;         
            unsigned ICP3:1;         
            unsigned ICP4:1;         
            unsigned ICP5:1;         
            unsigned ICP6:1;         
            unsigned ICP7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct INTCAPA_s INTCAPABITS;
volatile INTCAPABITS mcpINTCAPAbits = {{.value = 0x00}, .address = 0x10};
#define mcpINTCAPA   mcpINTCAPAbits.value



/************************* INTCAPB ********************************************/   
struct INTCAPB_s {
    union {
        struct {
            unsigned ICP0:1;         
            unsigned ICP1:1;         
            unsigned ICP2:1;         
            unsigned ICP3:1;         
            unsigned ICP4:1;         
            unsigned ICP5:1;         
            unsigned ICP6:1;         
            unsigned ICP7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct INTCAPB_s INTCAPBBITS;
volatile INTCAPBBITS mcpINTCAPBbits = {{.value = 0x00}, .address = 0x11};
#define mcpINTCAPB   mcpINTCAPBbits.value



/************************* GPIOA **********************************************/   
struct GPIOA_s {
    union {
        struct {
            unsigned GP0:1;         
            unsigned GP1:1;         
            unsigned GP2:1;         
            unsigned GP3:1;         
            unsigned GP4:1;         
            unsigned GP5:1;         
            unsigned GP6:1;         
            unsigned GP7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct GPIOA_s GPIOABITS;
volatile GPIOABITS mcpGPIOAbits = {{.value = 0x00}, .address = 0x12};
#define mcpGPIOA   mcpGPIOAbits.value


/************************* GPIOB **********************************************/   
struct GPIOB_s {
    union {
        struct {
            unsigned GP0:1;         
            unsigned GP1:1;         
            unsigned GP2:1;         
            unsigned GP3:1;         
            unsigned GP4:1;         
            unsigned GP5:1;         
            unsigned GP6:1;         
            unsigned GP7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct GPIOB_s GPIOBBITS;
volatile GPIOBBITS mcpGPIOBbits = {{.value = 0x00}, .address = 0x13};
#define mcpGPIOB   mcpGPIOBbits.value



/************************* OLATA **********************************************/   
struct OLATA_s {
    union {
        struct {
            unsigned OL0:1;         
            unsigned OL1:1;         
            unsigned OL2:1;         
            unsigned OL3:1;         
            unsigned OL4:1;         
            unsigned OL5:1;         
            unsigned OL6:1;         
            unsigned OL7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct OLATA_s OLATABITS;
volatile OLATABITS mcpOLATAbits = {{.value = 0x00}, .address = 0x14};
#define mcpOLATA   mcpOLATAbits.value


/************************* OLATB **********************************************/   
struct OLATB_s {
    union {
        struct {
            unsigned OL0:1;         
            unsigned OL1:1;         
            unsigned OL2:1;         
            unsigned OL3:1;         
            unsigned OL4:1;         
            unsigned OL5:1;         
            unsigned OL6:1;         
            unsigned OL7:1;         
        };
        uint8_t value;
    };
    uint8_t address;
};
typedef struct OLATB_s OLATBBITS;
volatile OLATBBITS mcpOLATBbits = {{.value = 0x00}, .address = 0x16};
#define mcpOLATB   mcpOLATBbits.value

#endif	/* MCP_MODULE_H */

