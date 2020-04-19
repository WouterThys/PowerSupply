#ifndef MCP_CONTROLLER_H
#define	MCP_CONTROLLER_H

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
    
/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/ 

    
/******************************************************************************/
/* Variables                                                                  */
/******************************************************************************/    

    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void mcpInit();

void mcpSetPORTA(uint8_t value);
void mcpSetPORTB(uint8_t value);

uint8_t mcpGetPORTA();
uint8_t mcpGetPORTB();

#endif	/* MCP_CONTROLLER_H */

