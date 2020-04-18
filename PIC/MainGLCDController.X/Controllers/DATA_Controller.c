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
static void idle    (StateMachine_T *sm, uint8_t input); // 0
static void command (StateMachine_T *sm, uint8_t input); // 1
static void v_set   (StateMachine_T *sm, uint8_t input); // 2
static void i_set   (StateMachine_T *sm, uint8_t input); // 3
static void v_read  (StateMachine_T *sm, uint8_t input); // 4
static void i_read  (StateMachine_T *sm, uint8_t input); // 5

static void done    (StateMachine_T *sm); // 6

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static dataCallback callback;

static GLCD_DataPacket_t data_packet;
static char v_set_arr[VALUE_LENGTH + 1];
static char i_set_arr[VALUE_LENGTH + 1];
static char v_rd_arr [VALUE_LENGTH + 1];
static char i_rd_arr [VALUE_LENGTH + 1];

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
    (state_machine.current_state)(&state_machine, data);
}

static void idle    (StateMachine_T *sm, uint8_t input) {
    if (DEBUG) printf("0");
    // Do work: Idle has no work
    
    // State machine next state
    if (input == UART_START_CHAR) {
        sm->current_state = command;
    }
}

static void command (StateMachine_T *sm, uint8_t input) {
    if (DEBUG) printf("1");
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
    if (DEBUG) printf("2");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }

    // State machine next state
    if (data_count < VALUE_LENGTH) {
        v_set_arr[data_count] = input;
        data_count++;
        sm->current_state = v_set;
    } else {
        v_set_arr[VALUE_LENGTH] = 0;
        data_count = 0;
        done(sm);
    }
}

static void i_set   (StateMachine_T *sm, uint8_t input) {
    if (DEBUG) printf("3");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }

    // State machine next state
    if (data_count < VALUE_LENGTH) {
        i_set_arr[data_count] = input;
        data_count++;
        sm->current_state = i_set;
    } else {
        i_set_arr[VALUE_LENGTH] = 0;
        data_count = 0;
        done(sm);
    }
}

static void v_read  (StateMachine_T *sm, uint8_t input) {
    if (DEBUG) printf("4");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }
    
    // State machine next state
    if (data_count < VALUE_LENGTH) {
        v_rd_arr[data_count] = input;
        data_count++;
        sm->current_state = v_read;
    } else {
        data_count = 0;
        v_rd_arr[VALUE_LENGTH] = 0;
        if (data_packet.command.command == CMD_SET_A_READ) {
            sm->current_state = i_read;
        } else {
            done(sm);
        }
    }

}

static void i_read  (StateMachine_T *sm, uint8_t input) {
    if (DEBUG) printf("5");
    if (input == UART_START_CHAR) {
        sm->current_state = command;
        return;
    }
    
    // State machine next state
    if (data_count < VALUE_LENGTH) {
        i_rd_arr[data_count] = input;
        data_count++;
        sm->current_state = i_read;
    } else {
        i_rd_arr[VALUE_LENGTH] = 0;
        data_count = 0;
        done(sm);
    }

}

static void done    (StateMachine_T *sm) {
    if (DEBUG) printf("6\n");
    
    // Do work: callback
    callback(&data_packet);
    
    // State machine next state
    sm->current_state = idle;
}


