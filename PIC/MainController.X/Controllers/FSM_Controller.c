#include "FSM_Controller.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/       
typedef struct MenuStateMachine MenuStateMachine_t;
typedef void (*FindNextMenuState)(MenuStateMachine_t *sm, Rotary_t *rotary);
typedef void (*HandleMenuState)(MenuStateMachine_t *sm, Rotary_t *rotary);
struct MenuStateMachine {
    FindNextMenuState state;
    HandleMenuState handle; 
};

typedef struct MainStateMachine MainStateMachine_t;
typedef void (*FindNextMainState)(MainStateMachine_t *sm);
typedef void (*HandleMainState)(MainStateMachine_t *sm);
struct MainStateMachine {
    FindNextMainState state;
    HandleMainState handle;
    MenuStateMachine_t * menu_fsm;
};


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile bool fsm_flag;
static Rotary_t rotary;    

static MainStateMachine_t main_fsm;
static MenuStateMachine_t menu_fsm;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initTimer();

static void state_ReadSupply(MainStateMachine_t *sm);
static void state_WriteSupply(MainStateMachine_t *sm);
static void state_UpdateMenu(MainStateMachine_t *sm);
static void state_WriteDebug(MainStateMachine_t *sm);

static void handle_ReadSupply(MainStateMachine_t *sm);
static void handle_WriteSupply(MainStateMachine_t *sm);
static void handle_UpdateMenu(MainStateMachine_t *sm);
static void handle_WriteDebug(MainStateMachine_t *sm);


static void state_PointToV(MenuStateMachine_t *sm, Rotary_t * rotary);
static void state_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary);
static void state_PointToI(MenuStateMachine_t *sm, Rotary_t * rotary);
static void state_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary);

static void handle_PointToV(MenuStateMachine_t *sm, Rotary_t * rotary);
static void handle_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary);
static void handle_PointToI(MenuStateMachine_t *sm, Rotary_t * rotary);
static void handle_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary);



static void initTimer() {
    
    T4CONbits.TON = 0;      // Disable
    T4CONbits.TCS = 0;      // Internal clock (Fp)
    T4CONbits.TGATE = 0;    // Gated time accumulation disabled
    T4CONbits.T32 = 0;      // 16-bit timer
    T4CONbits.TCKPS = 0b11; // 1:256 

    // Registers
    TMR4 = 0x0000; // Clear 
    float fT = 0;
    if (DEBUG) {
        fT = (float)FCY / 256; // Timer frequency = FCY / TCKPS
        PR4 = (4 * (float)T4_PERIOD / 1000) * fT; // Interrupt period Ti = PR4 * tT 
    } else {
        fT = (float)FCY / 256; // Timer frequency = FCY / TCKPS
        PR4 = ((float)T4_PERIOD / 1000) * fT; // Interrupt period Ti = PR4 * tT 
    }
        
    // Interrupts
    _T4IP = IP_TMR_4;
    _T4IF = 0; // Clear flag
    _T4IE = 1; // Enable interrupts

    // Enable timer
    T4CONbits.TON = 1;
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
    if(DEBUG) printf("main %d\n", 0);
}

static void handle_WriteSupply(MainStateMachine_t *sm) {
    if(DEBUG) printf("main %d\n", 1);
}

static void handle_UpdateMenu(MainStateMachine_t *sm) {
    if(DEBUG) printf("main %d\n", 2);
    // Get rotary data
    encGetRotaryData(ROTARY1, &rotary);
    // Find next state and handle menu FSM
    (sm->menu_fsm->state)(sm->menu_fsm, &rotary);
    if (*sm->menu_fsm->handle != NULL) {
        (sm->menu_fsm->handle)(sm->menu_fsm, &rotary);
    }
}

static void handle_WriteDebug(MainStateMachine_t *sm) {
    if(DEBUG) printf("main %d\n", 3);
}




static void state_PointToV(MenuStateMachine_t *sm, Rotary_t * rotary) {
    // Next state
    if (rotary->button == Pressed) {
        sm->state = state_SelectV;        
        sm->handle = handle_SelectV;
    } else if (rotary->turns != 0) {
        sm->state = state_PointToI;
        sm->handle = handle_PointToI;
    } else {
        sm->handle = NULL;
    }
}

static void state_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary) {
    // Next state
    if (rotary->button == Pressed) {
        sm->state = state_PointToV;
        sm->handle = handle_PointToV;
    } else if (rotary->turns != 0) {
        sm->state = state_SelectV; // Stay in this state 
        sm->handle = handle_SelectV; // Stay in this state 
    } else {
        sm->handle = NULL;
    }
}

static void state_PointToI(MenuStateMachine_t *sm, Rotary_t * rotary) {
    // Next state
    if (rotary->button == Pressed) {
        sm->state = state_SelectI;
        sm->handle = handle_SelectI;
    } else if (rotary->turns != 0) {
        sm->state = state_PointToV;
        sm->handle = handle_PointToV;
    } else {
        sm->handle = NULL;
    }
}

static void state_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary) {
    // Next state
    if (rotary->button == Pressed) {
        sm->state = state_PointToI;
        sm->handle = handle_PointToI;
    } else if (rotary->turns != 0) {
        sm->state = state_SelectI; // Stay in this state
        sm->handle = handle_SelectI; // Stay in this state
    } else {
        sm->handle = NULL;
    }
}


static void handle_PointToV(MenuStateMachine_t *sm, Rotary_t * rotary) {
    if(DEBUG) printf("point to v\n");
    glcdSelectMenu(0, true);
    glcdSetVoltageState(0, POINT_TO_V);
    glcdWriteMenu(0);
}

static void handle_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary) {
    if(DEBUG) printf("select V\n");
    glcdSelectMenu(0, true);
    glcdSetVoltageState(0, SELECT_V);
    glcdWriteMenu(0);
}

static void handle_PointToI(MenuStateMachine_t *sm, Rotary_t * rotary) {
    if(DEBUG) printf("point to i\n");
    glcdSelectMenu(0, true);
    glcdSetCurrentState(0, POINT_TO_I);
    glcdWriteMenu(0);
}

static void handle_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary) {
    if(DEBUG) printf("select V\n");
    glcdSelectMenu(0, true);
    glcdSetCurrentState(0, SELECT_I);
    glcdWriteMenu(0);
}

/*******************************************************************************
 *        DRIVER FUNCTIONS  
 ******************************************************************************/
void fsmInit() {
    
    // Setup timer
    initTimer();

    // Init FSMs
    menu_fsm.state = state_PointToV;
    main_fsm.state = state_ReadSupply; 
    main_fsm.menu_fsm = &menu_fsm;
}


void fsmExecute() {
    // Find next state
    (main_fsm.state)(&main_fsm);

    // Execute next state
    (main_fsm.handle)(&main_fsm); 
}

bool fsmShouldExecute() {
    bool should = fsm_flag;
    fsm_flag = false;
    return should;
}

/*******************************************************************************
 *       INTERRUPTS         
 ******************************************************************************/

void __attribute__ ( (interrupt, no_auto_psv) ) _T4Interrupt(void) {

    if (_T4IF) {
        fsm_flag = true; 
        _T4IF = 0; // Clear flag
    }
}
