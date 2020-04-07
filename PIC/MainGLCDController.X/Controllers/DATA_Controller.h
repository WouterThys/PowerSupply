#ifndef DATA_CONTROLLER_H
#define	DATA_CONTROLLER_H

#include <stdint.h>
#include "../settings.h"
#include "../Drivers/UART_Driver.h"

#define VALUE_LENGTH     6
#define UART_START_CHAR '$'

#define POINT_TO_V       0      /* Point to V   */
#define POINT_TO_I       1      /* Point to I   */
#define SELECT_V         2      /* Select V     */
#define SELECT_I         3      /* Select I     */


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define CMD_NONE         0      /* No data      */
#define CMD_SET_V_SET    1      /* Set voltage  */
#define CMD_SET_I_SET    2      /* Set current  */
#define CMD_SET_V_READ   3      /* Set read V   */
#define CMD_SET_I_READ   4      /* Set read I   */
#define CMD_SET_A_READ   5      /* Set both V&I */

typedef union Command {
    struct {
        uint8_t menu : 2;       // 2 bit menu selection
        uint8_t selected : 1;   // Menu is selected
        uint8_t pointSelect : 2;// Point & select option
        uint8_t command : 3;    // Command
    };
    uint8_t cmd_bits;
} Command_t;

typedef struct DataPacket {
    Command_t command;
    const char * v_set;         // Value for V
    const char * i_set;         // Value for I
    const char * v_rd;          // Read value for V
    const char * i_rd;          // Read value for I
} DataPacket_t;

typedef void (*dataCallback)(DataPacket_t * data);

/*******************************************************************************
 *          PROTOTYPES
 ******************************************************************************/

/**
 * Initialize with callback
 */
void dataInit(dataCallback callback);

/*
 * Read one byte
 */
void dataRead(uint8_t data);

#endif	/* DATA_CONTROLLER_H */

