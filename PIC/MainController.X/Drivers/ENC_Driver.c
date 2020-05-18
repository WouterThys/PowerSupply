#include "ENC_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
typedef struct {
    uint16_t prevAState   : 1;  // Previous state of line A
    uint16_t newAState    : 1;  // New state of line A
    uint16_t newBState    : 1;  // New state of line B
    uint16_t prevDState   : 1;  // Previous state of line button line
    uint16_t newDState    : 1;  // New state of button line
    Rotary_t * rotary;          // Pointer to corresponding rotary data
} RotaryState_t;

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          LOCAL FUNCTION PROTOTYPES
 ******************************************************************************/
static void readInputs();
static void checkChange();
static void handleRotary(RotaryState_t * rotary);
static void copyAndClear(Rotary_t * from, Rotary_t * too);

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static RotaryState_t rotary_state_1;
static RotaryState_t rotary_state_2;
static RotaryState_t rotary_state_3;

static Rotary_t rotary_1 = {0, Released};
static Rotary_t rotary_2 = {0, Released};
static Rotary_t rotary_3 = {0, Released};

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

static void readInputs() {
    
    // Read values
    rotary_state_1.newAState = ROT1_A_Pin;
    rotary_state_1.newBState = ROT1_B_Pin;
    rotary_state_1.newDState = ROT1_D_Pin;
    
}

static void checkChange() {
    handleRotary(&rotary_state_1);
    handleRotary(&rotary_state_2);
    handleRotary(&rotary_state_3);
}

static void handleRotary(RotaryState_t * rotary) {
    // Check rotary change
    if (rotary->newAState != rotary->prevAState) {
        // Check direction
        if (rotary->newBState != rotary->newAState) {
            rotary->rotary->turns++;
        } else {
            rotary->rotary->turns--;
        }
    }
    
    // Check button
    if (rotary->prevDState == 1 && rotary->newDState == 0) {   // Button is pressed
        rotary->rotary->button = Pressed;
    }
    
    // Copy back
    rotary->prevAState = rotary->newAState;
    rotary->prevDState = rotary->newDState;
}

static void copyAndClear(Rotary_t * from, Rotary_t * too) {
    // Copy values
    too->turns = from->turns;
    too->button = from->button;
    
    // Clear values
    from->turns = 0;
    from->button = Released;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void encDriverInit() {
    
    // Disable pull-up and pull-down resistors
    CNPUB = 0x0000;
    CNPDB = 0x0000;
    
    // Rotary 1
    ROT1_A_Dir = 1; // Input
    ROT1_B_Dir = 1;
    ROT1_D_Dir = 1;
    ROT1_A_Int = 1; // Enable Change Notification on Pin
    ROT1_B_Int = 0; // Disable Change Notification on Pin
    ROT1_D_Int = 1; // Enable Change Notification on Pin
    
    // Rotary 2
    ROT2_A_Dir = 1; // Input
    ROT2_B_Dir = 1;
    ROT2_D_Dir = 1;
    ROT2_A_Int = 1; // Enable Change Notification on Pin
    ROT2_B_Int = 0; // Disable Change Notification on Pin
    ROT2_D_Int = 1; // Enable Change Notification on Pin
    
    // Rotary 3
    ROT3_A_Dir = 1; // Input
    ROT3_B_Dir = 1;
    ROT3_D_Dir = 1;
    ROT3_A_Int = 1; // Enable Change Notification on Pin
    ROT3_B_Int = 0; // Disable Change Notification on Pin
    ROT3_D_Int = 1; // Enable Change Notification on Pin
    
    // Initialize
    rotary_state_1.prevAState = ROT1_A_Pin;
    rotary_state_1.prevDState = ROT1_D_Pin;
    rotary_state_1.rotary = &rotary_1;
    rotary_state_2.prevAState = ROT2_A_Pin;
    rotary_state_2.prevDState = ROT2_D_Pin;
    rotary_state_2.rotary = &rotary_2;
    rotary_state_3.prevAState = ROT3_A_Pin;
    rotary_state_3.prevDState = ROT3_D_Pin;
    rotary_state_3.rotary = &rotary_3;
    
    // Interrupts
    _CNIF = 0;      // Clear interrupt flag
    _CNIP = IP_CN;  // Interrupt priority
    _CNIE = 1;      // Enable interrupts
    
}

void encGetRotaryData(Rotary_t * data) {
    if (data == NULL) return;
    
    switch(data->id) {
        case ROTARY1:
            copyAndClear(rotary_state_1.rotary, data);
            break;
        case ROTARY2:
            copyAndClear(rotary_state_2.rotary, data);
            break;
        case ROTARY3:
            copyAndClear(rotary_state_3.rotary, data);
            break;
    }
}

/*******************************************************************************
 *          CN INTERRUPT
 ******************************************************************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _CNInterrupt(void) {
    if (_CNIF) {
        readInputs();
        checkChange();
        _CNIF = 0; // Clear interrupt
    }
}