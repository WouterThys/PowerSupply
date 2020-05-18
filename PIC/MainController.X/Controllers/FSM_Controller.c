#include "FSM_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
// Menu state machine updates GLCD display
typedef struct MenuStateMachine MenuStateMachine_t;
typedef void (*FindNextMenuState)(MenuStateMachine_t *sm, Rotary_t *rotary, GLKButton_t * button);
typedef void (*HandleMenuState)(MenuStateMachine_t *sm, Rotary_t *rotary, GLKButton_t * button);

struct MenuStateMachine {
    uint8_t menu_id;
    FindNextMenuState state;
    HandleMenuState handle;
};

// Main state machine goes through pulling data from supply boards etc
typedef struct MainStateMachine MainStateMachine_t;
typedef void (*FindNextMainState)(MainStateMachine_t *sm);
typedef void (*HandleMainState)(MainStateMachine_t *sm);

struct MainStateMachine {
    uint16_t supply_id;
    FindNextMainState state;
    HandleMainState handle;
    Rotary_t * rotary;
    SupplyStatus_t * supply_status;
    SupplyData_t * supply_data;
    MenuStateMachine_t * menu_fsm;
};


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile bool fsm_flag;
static GLKButton_t glcd_button;

static uint8_t current_fsm = 0;
static int8_t current_menu = 0;
static MainStateMachine_t main_fsm[3];
static MenuStateMachine_t menu_fsm[3];

// Extra data
static Rotary_t rotaries[3];

// Supply data
static SupplyStatus_t supply_status[3];
static SupplyData_t supply_data[3]; 

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initTimer();
static void initRotary(uint8_t id, Rotary_t * rotary);
static void initSupplyData(SupplyData_t * data, uint16_t defaultV, uint16_t defaultI);
static void initFSM(uint8_t id, MainStateMachine_t *mainFsm, MenuStateMachine_t *menuFsm);

static void readInputs(MainStateMachine_t *sm);
static void onButtonPressed(GLKButton_t button);
static void onSupplyError(Error_t error);

static void state_ReadSupply(MainStateMachine_t *sm);
static void state_WriteSupply(MainStateMachine_t *sm);
static void state_UpdateMenu(MainStateMachine_t *sm);
static void state_WriteDebug(MainStateMachine_t *sm);

static void handle_ReadSupply(MainStateMachine_t *sm);
static void handle_WriteSupply(MainStateMachine_t *sm);
static void handle_UpdateMenu(MainStateMachine_t *sm);
static void handle_WriteDebug(MainStateMachine_t *sm);

static void state_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t  * button);
static void state_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t  * button);

static void handle_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t  * button);
static void handle_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t  * button);

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
        PR4 = (4 * (float) T4_PERIOD / 1000) * fT; // Interrupt period Ti = PR4 * tT 
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

static void initSupplyData(SupplyData_t * data, uint16_t defaultV, uint16_t defaultI) {
    data->setVoltage.value = defaultV;
    data->setVoltage.changed = true;
    data->setCurrent.value = defaultI;
    data->setCurrent.changed = true;
    data->msrVoltage.value = 0;
    data->msrCurrent.value = 0;
    data->msrTemperature.value = 0;
}

static void initFSM(uint8_t id, MainStateMachine_t *mainFsm, MenuStateMachine_t *menuFsm) {
    menuFsm->menu_id = id;
    menuFsm->state = state_SelectV;
    menuFsm->handle = NULL;
    mainFsm->state = state_ReadSupply;
    mainFsm->handle = NULL;
    mainFsm->rotary = &(rotaries[id]);
    mainFsm->supply_status = &(supply_status[id]);
    mainFsm->supply_data = &(supply_data[id]);
    mainFsm->menu_fsm = &(menu_fsm[id]);
}

static void readInputs(MainStateMachine_t *sm) {
    
    // Update current menu with GLCD button press
    if (glcd_button == ButtonLeft) {
        current_menu--;
        if (current_menu < SUPPLY_1) {
            current_menu = SUPPLY_3;
        }
        glcd_button = 0;
    } 
    
    if (glcd_button == ButtonRight) {
        current_menu++;
        if (current_menu > SUPPLY_3) {
            current_menu = SUPPLY_1;
        }
        glcd_button = 0;
    }
    
    // Get rotary data
    encGetRotaryData(sm->rotary);
}

static void onButtonPressed(GLKButton_t button) {
    glcd_button = button;
}

static void onSupplyError(Error_t error) {
    
}


static void state_ReadSupply(MainStateMachine_t *sm) {
    sm->state = state_WriteSupply;
    sm->handle = handle_WriteSupply;
}

static void state_WriteSupply(MainStateMachine_t *sm) {
    sm->state = state_UpdateMenu;
    sm->handle = handle_UpdateMenu;
}

static void state_UpdateMenu(MainStateMachine_t *sm) {
    sm->state = state_WriteDebug;
    sm->handle = handle_WriteDebug;
}

static void state_WriteDebug(MainStateMachine_t *sm) {
    sm->state = state_ReadSupply;
    sm->handle = handle_ReadSupply;
}

static void handle_ReadSupply(MainStateMachine_t *sm) {
    if (DEBUG) printf("main %d\n", 0);
}

static void handle_WriteSupply(MainStateMachine_t *sm) {
    if (DEBUG) printf("main %d\n", 1);
    
    // Rotary updates voltage setting
    if (sm->supply_id == current_menu) {
        int16_t turns = sm->rotary->turns;
        if (turns != 0) {

            if (rotary->turns > 0) {

            } else {

            }
        }
    }
}

static void handle_UpdateMenu(MainStateMachine_t *sm) {
    if (DEBUG) printf("main %d\n", 2);
    
    // Find next state and handle menu FSM
    (sm->menu_fsm->state)(sm->menu_fsm, sm->rotary, &glcd_button);
    if (*sm->menu_fsm->handle != NULL) {
        (sm->menu_fsm->handle)(sm->menu_fsm, sm->rotary, &glcd_button);
    }
}

static void handle_WriteDebug(MainStateMachine_t *sm) {
    if (DEBUG) printf("main %d\n", 3);

    // Set next menu
    switch (current_fsm) {
        case SUPPLY_1: current_fsm = SUPPLY_2;
            break;
        case SUPPLY_2: current_fsm = SUPPLY_3;
            break;
        case SUPPLY_3: current_fsm = SUPPLY_1;
            break;
    }
}

static void state_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t * button) {
    // Next state
    if (current_fsm == current_menu) {
        menuSelect(sm->menu_id);
        if (*button == ButtonTop || *button == ButtonBottom) {
            // Move to next state
            sm->state = state_SelectI;
            sm->handle = handle_SelectI;
        } else if (rotary->turns != 0) {
            // Execute this state again
            sm->state = state_SelectV;
            sm->handle = handle_SelectV;
        } else {
            // Do nothing
            sm->handle = NULL;
        }
    } else {
        sm->handle = NULL;
    }
}

static void state_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t * button) {
    // Next state
    if (current_fsm == current_menu) {
        menuSelect(sm->menu_id);
        if (*button == ButtonTop || *button == ButtonBottom) {
            // Move to next state
            sm->state = state_SelectV;
            sm->handle = handle_SelectV;
        } else if (rotary->turns != 0) {
            // Execute this state again
            sm->state = state_SelectI;
            sm->handle = handle_SelectI;
        } else {
            // Do nothing
            sm->handle = NULL;
        }
    } else {
        sm->handle = NULL;
    }
    
}

static void handle_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t * button) {
    if (DEBUG) printf("select V\n");

    // Handle button press
    if (*button != 0) {
        menuSetVoltageState(sm->menu_id, STATE_POINT);
        menuSetCurrentState(sm->menu_id, STATE_NONE);
    }
    
    

    // Reset values
    *button = 0;
}

static void handle_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary, GLKButton_t * button) {
    if (DEBUG) printf("select V\n");

    menuSetCurrentState(sm->menu_id, STATE_POINT);
    menuSetVoltageState(sm->menu_id, STATE_NONE);
    
    // Rotary updates current setting
    
    // Reset values
    *button = 0;
}

/*******************************************************************************
 *        DRIVER FUNCTIONS  
 ******************************************************************************/
void fsmInit() {

    // Setup timer
    initTimer();
    
    // Initialize rotary driver
    encDriverInit();
    
    // Initialize supplies controller
    // TODO: multiple supplies!!!!!!!!!
    splInit(&(supply_status[0]), &onSupplyError);
    
    // Initialize rotary-encoders
    initRotary(SUPPLY_1, &(rotaries[SUPPLY_1]));
    initRotary(SUPPLY_2, &(rotaries[SUPPLY_2]));
    initRotary(SUPPLY_3, &(rotaries[SUPPLY_3]));
    
    // Initialize supply-data
    initSupplyData(&(supply_data[SUPPLY_1]), DEFAULT_V, DEFAULT_I);
    initSupplyData(&(supply_data[SUPPLY_2]), DEFAULT_V, DEFAULT_I);
    initSupplyData(&(supply_data[SUPPLY_3]), DEFAULT_V, DEFAULT_I);

    // Initialize FSMs
    initFSM(SUPPLY_1, &(main_fsm[SUPPLY_1]), &(menu_fsm[SUPPLY_1]));
    initFSM(SUPPLY_2, &(main_fsm[SUPPLY_2]), &(menu_fsm[SUPPLY_2]));
    initFSM(SUPPLY_3, &(main_fsm[SUPPLY_3]), &(menu_fsm[SUPPLY_3]));
    
    // Initialize menu
    menuInit(&onButtonPressed);
    menuSetVoltageState(SUPPLY_3, STATE_POINT);
    menuSetVoltageState(SUPPLY_2, STATE_POINT);
    menuSetVoltageState(SUPPLY_1, STATE_POINT);
}

void fsmExecute() {

    // Read inputs
    readInputs(&main_fsm[current_fsm]);
    
    // Find next state
    (main_fsm[current_fsm].state)(&main_fsm[current_fsm]);

    // Handle state
    (main_fsm[current_fsm].handle)(&main_fsm[current_fsm]);
}

bool fsmShouldExecute() {
    bool should = fsm_flag;
    fsm_flag = false;
    return should;
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
