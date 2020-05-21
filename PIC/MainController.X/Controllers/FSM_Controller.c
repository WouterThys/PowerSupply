#include "FSM_Controller.h"

/*******************************************************************************
 *          FSM
 ******************************************************************************/

typedef struct MenuState {
    uint16_t supply_id; // Supply id for this menu
    bool updateSelection; // Indicate if update is needed
    bool selected; // Is menu selected
    bool voltage; // Set voltage = true, set current = false
} MenuState_t;

// State machine goes through pulling data from supply boards etc
typedef struct StateMachine StateMachine_t;

// State transition function prototype
typedef void (*state_transition)(StateMachine_t *sm);

// State handle function prototype
typedef void (*state_handler)(StateMachine_t *sm);

// Actual FSM data

struct StateMachine {
    // ID
    uint16_t supply_id; // Id of supply 

    // State update
    state_transition transition; // Transition to next state 
    state_handler handler; // Handler for the state

    // Data
    Rotary_t * rotary; // Rotary input for this supply
    MenuState_t * menu_state; // State of the menu for this supply
    SupplyStatus_t * supply_status; // Status for this supply
    SupplyData_t * supply_data; // Data of this supply
};

// State transitions
static void transitionReadSupply(StateMachine_t * sm);
static void transitionRotary(StateMachine_t * sm);
static void transitionWriteSupply(StateMachine_t * sm);
static void transitionUpdateGLCD(StateMachine_t * sm);
static void transitionDebug(StateMachine_t * sm);

// State handlers
static void handleReadSupply(StateMachine_t * sm);
static void handleRotary(StateMachine_t * sm);
static void handleWriteSupply(StateMachine_t * sm);
static void handleUpdateGLCD(StateMachine_t * sm);
static void handleDebug(StateMachine_t * sm);

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile bool fsm_flag;
static volatile uint8_t button;

static volatile uint16_t current_fsm;
static volatile int16_t current_supply;

static StateMachine_t fsms[3];
static Rotary_t rotaries[3];
static MenuState_t menus[3];
static SupplyStatus_t supply_status[3];
static SupplyData_t supply_data[3];

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initTimer();
static void initRotary(uint8_t id, Rotary_t * rotary);
static void initSupplyData(uint8_t id, SupplyData_t * data, uint16_t address, uint16_t defaultV, uint16_t defaultI);
static void initSupplyStatus(SupplyStatus_t * status);
static void initMenu(uint8_t id, MenuState_t * menu);
static void initFSM(uint8_t id, StateMachine_t *sm);
static void handleButton();

// Callback
static void onButtonPressed(uint8_t btn);
static void onUartDataRead(uint8_t data);
static void onSupplyError(Error_t error);

static void initTimer() {

    T4CONbits.TON = 0; // Disable
    T4CONbits.TCS = 0; // Internal clock (Fp)
    T4CONbits.TGATE = 0; // Gated time accumulation disabled
    T4CONbits.T32 = 0; // 16-bit timer
    T4CONbits.TCKPS = 0b11; // 1:256 

    // Registers
    TMR4 = 0x0000; // Clear 
    float fT = 0;
    if (DEBUG) {
        fT = (float) FCY / 256; // Timer frequency = FCY / TCKPS
        PR4 = (2 * (float) T4_PERIOD / 1000) * fT; // Interrupt period Ti = PR4 * tT 
    } else {
        fT = (float) FCY / 256; // Timer frequency = FCY / TCKPS
        PR4 = ((float) T4_PERIOD / 1000) * fT; // Interrupt period Ti = PR4 * tT 
    }

    // Interrupts
    _T4IP = IP_TMR_4;
    _T4IF = 0; // Clear flag
    _T4IE = 1; // Enable interrupts

    // Enable timer
    T4CONbits.TON = 1;
}

static void initRotary(uint8_t id, Rotary_t * rotary) {
    rotary->id = id;
    rotary->button = Released;
    rotary->turns = 0;
}

static void initSupplyData(uint8_t id, SupplyData_t * data, uint16_t address, uint16_t defaultV, uint16_t defaultI) {
    data->supply_id = id;
    data->i2c_address = address;

    data->set_voltage.value = defaultV;
    data->set_voltage.changed = true;
    data->set_current.value = defaultI;
    data->set_current.changed = true;
    data->msr_voltage.value = 0;
    data->msr_current.value = 0;
    data->msr_temperature.value = 0;
}

static void initSupplyStatus(SupplyStatus_t * status) {
    status->value = 0x0000;
}

static void initFSM(uint8_t id, StateMachine_t *sm) {
    sm->supply_id = id;
    sm->transition = transitionReadSupply;
    sm->handler = NULL;
    sm->menu_state = &(menus[id]);
    sm->rotary = &(rotaries[id]);
    sm->supply_status = &(supply_status[id]);
    sm->supply_data = &(supply_data[id]);
}

static void initMenu(uint8_t id, MenuState_t * menu) {
    menu->supply_id = id;
    menu->updateSelection = false;
    menu->selected = false;
    menu->voltage = true;
}

static void handleButton() {

    if (button != 0) {

        menus[current_supply].selected = false;
        menus[current_supply].updateSelection = false;

        switch (button) {
            default:
                dbgPrintLcdCallback(button);
                break;

            case BTN_LEFT:
                current_supply--;
                if (current_supply < SUPPLY_1) {
                    current_supply = SUPPLY_3;
                }
                menus[current_supply].selected = true;
                menus[current_supply].updateSelection = true;
                break;

            case BTN_RIGHT:
                current_supply++;
                if (current_supply > SUPPLY_3) {
                    current_supply = SUPPLY_1;
                }
                menus[current_supply].selected = true;
                menus[current_supply].updateSelection = true;
                break;

            case BTN_TOP:
            case BTN_BOTTOM:
                if (menus[current_supply].voltage) {
                    menus[current_supply].voltage = false;
                } else {
                    menus[current_supply].voltage = true;
                }
                menus[current_supply].updateSelection = true;
                break;
        }
        button = 0;
    }
}

static void onButtonPressed(uint8_t btn) {
    button = btn;
}

static void onUartDataRead(uint8_t data) {

}

static void onSupplyError(Error_t error) {
    if (error.hasError) {
        if (error.source == ES_I2C) {
            dbgPrintI2CError(current_fsm, error.code);
        }
    }
}

static void transitionReadSupply(StateMachine_t * sm) {
    sm->transition = transitionRotary;
    sm->handler = handleReadSupply;
}

static void transitionRotary(StateMachine_t * sm) {
    sm->transition = transitionWriteSupply;
    sm->handler = handleRotary;
}

static void transitionWriteSupply(StateMachine_t * sm) {
    sm->transition = transitionUpdateGLCD;
    sm->handler = handleWriteSupply;
}

static void transitionUpdateGLCD(StateMachine_t * sm) {
    sm->transition = transitionDebug;
    sm->handler = handleUpdateGLCD;
}

static void transitionDebug(StateMachine_t * sm) {
    sm->transition = transitionReadSupply;
    sm->handler = handleDebug;
}

// State handlers

static void handleReadSupply(StateMachine_t * sm) {
    //if (sm->supply_id != SUPPLY_1) return;

    splUpdateData(sm->supply_status, sm->supply_data);
}

static void handleRotary(StateMachine_t * sm) {
    encGetRotaryData(sm->rotary);
    int16_t turns = sm->rotary->turns;
    if (turns != 0) {
        if (sm->menu_state->voltage) {
            int16_t change = turns * VOLTAGE_STEP;
            int16_t value = sm->supply_data->set_voltage.value;
            value += change;
            if (value < VOLTAGE_MIN) {
                value = VOLTAGE_MIN;
            }
            if (value > VOLTAGE_MAX) {
                value = VOLTAGE_MAX;
            }
            sm->supply_data->set_voltage.value = (uint16_t) value;
            sm->supply_data->set_voltage.changed = true;

        } else {
            int16_t value = sm->supply_data->set_current.value;
            int16_t change = 0;
            if (turns > 0) {
                if (value >= CURRENT_MAX_STEP) {
                    change = turns * CURRENT_MAX_STEP;
                } else {
                    change = turns * CURRENT_MIN_STEP;
                }
            } else {
                if (value > CURRENT_MAX_STEP) {
                    change = turns * CURRENT_MAX_STEP;
                } else {
                    change = turns * CURRENT_MIN_STEP;
                }
            }

            value += change;
            if (value < CURRENT_MIN) {
                value = CURRENT_MIN;
            }
            if (value > CURRENT_MAX) {
                value = CURRENT_MAX;
            }
            sm->supply_data->set_current.value = (uint16_t) value;
            sm->supply_data->set_current.changed = true;

        }
    }
}

static void handleWriteSupply(StateMachine_t * sm) {
    //if (sm->supply_id != SUPPLY_1) return;

    if (sm->supply_data->set_voltage.changed) {
        splWriteVoltage(sm->supply_data);
    }

    if (sm->supply_data->set_current.changed) {
        splWriteCurrent(sm->supply_data);
    }
}

static void handleUpdateGLCD(StateMachine_t * sm) {
    // Update GLCD
    if (sm->menu_state->updateSelection) {
        if (sm->menu_state->voltage) {
            menuSetVoltageState(sm->supply_id, STATE_POINT);
            menuSetCurrentState(sm->supply_id, STATE_NONE);
        } else {
            menuSetVoltageState(sm->supply_id, STATE_NONE);
            menuSetCurrentState(sm->supply_id, STATE_POINT);
        }
        sm->menu_state->updateSelection = false;
    }

    if (sm->supply_data->set_voltage.changed) {
        menuSetVoltageSet(sm->supply_id, sm->supply_data->set_voltage.value);
        sm->supply_data->set_voltage.changed = false;
    }

    if (sm->supply_data->set_current.changed) {
        menuSetCurrentSet(sm->supply_id, sm->supply_data->set_current.value);
        sm->supply_data->set_current.changed = false;
    }

    if (sm->supply_data->msr_voltage.changed) {
        uint16_t voltage = sm->supply_data->msr_voltage.value;
        menuSetVoltageRead(sm->supply_id, digitalToVoltage(voltage));
        sm->supply_data->msr_voltage.changed = false;
    }

    if (sm->supply_data->msr_current.changed) {
        uint16_t current = sm->supply_data->msr_current.value;
        menuSetCurrentRead(sm->supply_id, digitalToCurrent(current));
        sm->supply_data->msr_current.changed = false;
    }

    current_fsm++;
    if (current_fsm > 2) {
        current_fsm = 0;
    }
}

static void handleDebug(StateMachine_t * sm) {
    dbgPrintSupplyStatus(sm->supply_id, *(sm->supply_status));
    dbgPrintSupplyMeasurements(sm->supply_id,
            sm->supply_data->msr_voltage.value,
            sm->supply_data->msr_current.value,
            sm->supply_data->msr_temperature.value);
}

/*******************************************************************************
 *        DRIVER FUNCTIONS  
 ******************************************************************************/
void fsmInit() {

    // For debugging
    uart1DriverInit(UART1_BAUD, &onUartDataRead);
    uart1DriverEnable(true);

    // Setup timer
    initTimer();

    // Initialize rotary driver
    encDriverInit();

    // Initialize supplies controller
    splInit(&onSupplyError);

    // Initialize rotary-encoders
    initRotary(SUPPLY_1, &(rotaries[SUPPLY_1]));
    initRotary(SUPPLY_2, &(rotaries[SUPPLY_2]));
    initRotary(SUPPLY_3, &(rotaries[SUPPLY_3]));

    // Initialize supply-data
    initSupplyData(SUPPLY_1, &(supply_data[SUPPLY_1]), I2C_ADDRESS_1, DEFAULT_V_1, DEFAULT_I_1);
    initSupplyData(SUPPLY_2, &(supply_data[SUPPLY_2]), I2C_ADDRESS_2, DEFAULT_V_2, DEFAULT_I_2);
    initSupplyData(SUPPLY_3, &(supply_data[SUPPLY_3]), I2C_ADDRESS_3, DEFAULT_V_3, DEFAULT_I_3);

    // Initialize supply-status
    initSupplyStatus(&(supply_status[SUPPLY_1]));
    initSupplyStatus(&(supply_status[SUPPLY_2]));
    initSupplyStatus(&(supply_status[SUPPLY_3]));

    // Initialize menu-states
    initMenu(SUPPLY_1, &(menus[SUPPLY_1]));
    initMenu(SUPPLY_2, &(menus[SUPPLY_2]));
    initMenu(SUPPLY_3, &(menus[SUPPLY_3]));

    // Initialize FSMs
    initFSM(SUPPLY_1, &(fsms[SUPPLY_1]));
    initFSM(SUPPLY_2, &(fsms[SUPPLY_2]));
    initFSM(SUPPLY_3, &(fsms[SUPPLY_3]));

    // Initialize menu
    menuInit(&onButtonPressed);
    menuSetVoltageState(SUPPLY_3, STATE_POINT);
    menuSetVoltageState(SUPPLY_2, STATE_POINT);
    menuSetVoltageState(SUPPLY_1, STATE_POINT);

    current_supply = SUPPLY_1;
    current_fsm = 0;

    // Debug
    if (DEBUG_FSM) {
        printf("Ready!\n");
    }
}

void fsmExecute() {

    PORTBbits.RB7 = !PORTBbits.RB7;

    // Button
    handleButton();

    // Transition
    (fsms[current_fsm].transition)(&fsms[current_fsm]);

    // Handle
    (fsms[current_fsm].handler)(&fsms[current_fsm]);

    // Clear
    fsm_flag = false;
}

bool fsmShouldExecute() {
    return fsm_flag;
}

/*******************************************************************************
 *       INTERRUPTS         
 ******************************************************************************/

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {
    if (_T4IF) {
        fsm_flag = true;
        _T4IF = 0; // Clear flag
    }
}
