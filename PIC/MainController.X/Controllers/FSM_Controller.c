#include "FSM_Controller.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/       
typedef struct MainStateMachine MainStateMachine_t;
typedef void (*HandleMainState)(MainStateMachine_t *sm);
struct MainStateMachine { HandleMainState current_state; };

typedef struct MenuStateMachine MenuStateMachine_t;
typedef void (*HandleMenuState)(MenuStateMachine_t *sm, Rotary_t *rotary);
struct MenuStateMachine { HandleMenuState current_state; };

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile bool fsm_flag;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initTimer();

static void main_ReadSupply(MainStateMachine_t *sm);
static void main_WriteSupply(MainStateMachine_t *sm);
static void main_UpdateMenu(MainStateMachine_t *sm);
static void main_WriteDebug(MainStateMachine_t *sm);

static void menu_PointToV(MenuStateMachine_t *sm, Rotary_t * rotary);
static void menu_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary);
static void menu_PointToI(MenuStateMachine_t *sm, Rotary_t * rotary);
static void menu_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary);


static void initTimer() {
    
    T4CONbits.TON = 0;      // Disable
    T4CONbits.TCS = 0;      // Internal clock (Fp)
    T4CONbits.TGATE = 0;    // Gated time accumulation disabled
    T4CONbits.T32 = 0;      // 16-bit timer
    T4CONbits.TCKPS = 0b11; // 1:256 

    // Registers
    TMR4 = 0x0000; // Clear 
    float fT = 0;
    float tT = 0;
    if (DEBUG) {
        fT = (float)FCY / 256; // Timer frequency = FCY / TCKPS
        PR4 = (4 * (float)T4_PERIOD / 1000) * fT; // Interrupt period Ti = PR4 * tT 
        printf("PR4 = %d\n", PR4);
    } else {
        fT = (float)FCY / 256; // Timer frequency = FCY / TCKPS
        tT = 1 / fT;    // Timer period
        PR4 = (T4_PERIOD / 1000) / tT; // Interrupt period Ti = PR4 * tT 
        printf("PR4 = %d\n", PR4);
    }
        
    // Interrupts
    _T4IP = IP_TMR_4;
    _T4IF = 0; // Clear flag
    _T4IE = 1; // Enable interrupts

    // Enable timer
    T4CONbits.TON = 1;
}

static void main_ReadSupply(MainStateMachine_t *sm) {
    // TODO: Read from supply
    
    // Next state
    sm->current_state = main_WriteSupply;
}

static void main_WriteSupply(MainStateMachine_t *sm) {
    // TODO: Write to supply board
    
    // Next state
    sm->current_state = main_UpdateMenu;
}

static void main_UpdateMenu(MainStateMachine_t *sm) {
    // TODO: Update menu with menu fsm
    
    // Next state
    sm->current_state = main_WriteDebug;
}

static void main_WriteDebug(MainStateMachine_t *sm) {
    // TODO: Write stuff to UART debug
    
    // Next state
    sm->current_state = main_ReadSupply;
}




static void menu_PointToV(MenuStateMachine_t *sm, Rotary_t * rotary) {
    // 

    // Next state
    if (rotary->button == Pressed) {
        sm->current_state = menu_SelectV;        
    } else if (rotary->turns != 0) {
        sm->current_state = menu_PointToI;
    }
}

static void menu_SelectV(MenuStateMachine_t *sm, Rotary_t * rotary) {


    // Next state
    if (rotary->button == Pressed) {
        sm->current_state = menu_PointToV;
    } else if (rotary->turns != 0) {
        sm->current_state = menu_SelectV; // Stay in this state 
    }
}

static void menu_PointToI(MenuStateMachine_t *sm, Rotary_t * rotary) {


    // Next state
    if (rotary->button == Pressed) {
        sm->current_state = menu_SelectI;
    } else if (rotary->turns != 0) {
        sm->current_state = menu_PointToV;
    }
}

static void menu_SelectI(MenuStateMachine_t *sm, Rotary_t * rotary) {


    // Next state
    if (rotary->button == Pressed) {
        sm->current_state = menu_PointToI;
    } else if (rotary->turns != 0) {
        sm->current_state = menu_SelectI; // Stay in this state
    }
}


/*******************************************************************************
 *        DRIVER FUNCTIONS  
 ******************************************************************************/
void fsmInit() {
    
    // Setup timer
    initTimer();
}


void fsmExecute() {

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
