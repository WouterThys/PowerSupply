#include "DATA_Controller.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
// Forward declaration of state machine struct
typedef struct StateMachine StateMachine_T;

// Function pointer to handle state machine states
typedef void (*HandleState)(StateMachine_T *sm, uint8_t input);

// Actual StateMachine struct
struct StateMachine {
    HandleState current_state;
};

// State handlers
static void idle    (StateMachine_T *sm, uint8_t input);
static void command (StateMachine_T *sm, uint8_t input);
static void v_set   (StateMachine_T *sm, uint8_t input);
static void i_set   (StateMachine_T *sm, uint8_t input);
static void v_read  (StateMachine_T *sm, uint8_t input);
static void i_read  (StateMachine_T *sm, uint8_t input);

static void done    (StateMachine_T *sm);

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static dataCallback callback;

static GLCD_DataPacket_t data_packet;
static char v_set_arr[VALUE_LENGTH];
static char i_set_arr[VALUE_LENGTH];
static char v_rd_arr[VALUE_LENGTH];
static char i_rd_arr[VALUE_LENGTH];

static StateMachine_T state_machine = { idle };
static uint8_t data_count;

/*******************************************************************************
 *          FUNCTIONS
 ******************************************************************************/

static void printState(const char * state);

void dataInit(dataCallback cb) {
    callback = cb;

    // Initialize data
    data_packet.command.cmd_bits = 0; // Clear all
    data_packet.v_set = v_set_arr;
    data_packet.i_set = i_set_arr;
    data_packet.v_rd = v_rd_arr;
    data_packet.i_rd = i_rd_arr;

    // UART init
    UART_Init(UART_BAUD, UART_INVERT, dataRead);

    // State Machine Init
    state_machine.current_state = idle;
    (state_machine.current_state)(&state_machine, 0);
}

void dataRead(uint8_t data) {
    // Handle state
    LED1 = !LED1;
    (state_machine.current_state)(&state_machine, data);
}

static void printState(const char * state) {
    printf("s=%s\n", state);
}


static void idle    (StateMachine_T *sm, uint8_t input) {
    printState("Idle");
    // Do work: Idle has no work
    
    // State machine next state
    if (input == UART_START_CHAR) {
        sm->current_state = command;
    }
}

static void command (StateMachine_T *sm, uint8_t input) {
    printState("Command");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }

    // Do work: update command
    data_packet.command.cmd_bits = input;
    data_count = 0;

    // State machine next state
    switch(data_packet.command.command) {
        default:
        case CMD_NONE:
            done(sm);
            break;
        case CMD_SET_V_SET:
            sm->current_state = v_set;
            break;
        case CMD_SET_I_SET:
            sm->current_state = i_set;
            break;
        case CMD_SET_V_READ:
            sm->current_state = v_read;
            break;
        case CMD_SET_I_READ:
            sm->current_state = i_read;
            break;
        case CMD_SET_A_READ:
            sm->current_state = v_read;
            break;

    }
}

static void v_set   (StateMachine_T *sm, uint8_t input) {
    printState("v_set");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }

    // Do work: fill v_set buffer
    v_set_arr[data_count] = input;

    // State machine next state
    if (data_count < VALUE_LENGTH) {
        data_count++;
        sm->current_state = v_set;
    } else {
        data_count = 0;
        done(sm);
    }
}

static void i_set   (StateMachine_T *sm, uint8_t input) {
    printState("i_set");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }

    // Do work: fill i_set buffer
    i_set_arr[data_count] = input;

    // State machine next state
    if (data_count < VALUE_LENGTH) {
        data_count++;
        sm->current_state = i_set;
    } else {
        data_count = 0;
        done(sm);
    }
}

static void v_read  (StateMachine_T *sm, uint8_t input) {
    printState("v_read");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }
    
    // Do work: fill v_set buffer
    v_rd_arr[data_count] = input;

    // State machine next state
    if (data_count < VALUE_LENGTH) {
        data_count++;
        sm->current_state = v_read;
    } else {
        data_count = 0;
        if (data_packet.command.command == CMD_SET_A_READ) {
            sm->current_state = i_read;
        } else {
            done(sm);
        }
    }

}

static void i_read  (StateMachine_T *sm, uint8_t input) {
    printState("i_read");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }

    // Do work: fill v_set buffer
    i_rd_arr[data_count] = input;

    // State machine next state
    if (data_count < VALUE_LENGTH) {
        data_count++;
        sm->current_state = i_read;
    } else {
        data_count = 0;
        done(sm);
    }

}

static void done    (StateMachine_T *sm) {
    printState("Done");
    
    // Do work: callback
    callback(&data_packet);
    
    // State machine next state
    sm->current_state = idle;
}


