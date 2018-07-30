#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=Controllers/MCP_Controller.c Controllers/MENU_Controller.c Controllers/SUPPLIES_Controller.c Controllers/UART_Controller.c Drivers/ENC_Driver.c Drivers/I2C_Driver.c Drivers/LCD_Driver.c Drivers/SPI1_Driver.c Drivers/SPI_Driver.c Drivers/SYSTEM_Driver.c Drivers/UART_Driver.c configuration.c main.c utils.c FSM_Controller.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Controllers/MCP_Controller.o ${OBJECTDIR}/Controllers/MENU_Controller.o ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o ${OBJECTDIR}/Controllers/UART_Controller.o ${OBJECTDIR}/Drivers/ENC_Driver.o ${OBJECTDIR}/Drivers/I2C_Driver.o ${OBJECTDIR}/Drivers/LCD_Driver.o ${OBJECTDIR}/Drivers/SPI1_Driver.o ${OBJECTDIR}/Drivers/SPI_Driver.o ${OBJECTDIR}/Drivers/SYSTEM_Driver.o ${OBJECTDIR}/Drivers/UART_Driver.o ${OBJECTDIR}/configuration.o ${OBJECTDIR}/main.o ${OBJECTDIR}/utils.o ${OBJECTDIR}/FSM_Controller.o
POSSIBLE_DEPFILES=${OBJECTDIR}/Controllers/MCP_Controller.o.d ${OBJECTDIR}/Controllers/MENU_Controller.o.d ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o.d ${OBJECTDIR}/Controllers/UART_Controller.o.d ${OBJECTDIR}/Drivers/ENC_Driver.o.d ${OBJECTDIR}/Drivers/I2C_Driver.o.d ${OBJECTDIR}/Drivers/LCD_Driver.o.d ${OBJECTDIR}/Drivers/SPI1_Driver.o.d ${OBJECTDIR}/Drivers/SPI_Driver.o.d ${OBJECTDIR}/Drivers/SYSTEM_Driver.o.d ${OBJECTDIR}/Drivers/UART_Driver.o.d ${OBJECTDIR}/configuration.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/utils.o.d ${OBJECTDIR}/FSM_Controller.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Controllers/MCP_Controller.o ${OBJECTDIR}/Controllers/MENU_Controller.o ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o ${OBJECTDIR}/Controllers/UART_Controller.o ${OBJECTDIR}/Drivers/ENC_Driver.o ${OBJECTDIR}/Drivers/I2C_Driver.o ${OBJECTDIR}/Drivers/LCD_Driver.o ${OBJECTDIR}/Drivers/SPI1_Driver.o ${OBJECTDIR}/Drivers/SPI_Driver.o ${OBJECTDIR}/Drivers/SYSTEM_Driver.o ${OBJECTDIR}/Drivers/UART_Driver.o ${OBJECTDIR}/configuration.o ${OBJECTDIR}/main.o ${OBJECTDIR}/utils.o ${OBJECTDIR}/FSM_Controller.o

# Source Files
SOURCEFILES=Controllers/MCP_Controller.c Controllers/MENU_Controller.c Controllers/SUPPLIES_Controller.c Controllers/UART_Controller.c Drivers/ENC_Driver.c Drivers/I2C_Driver.c Drivers/LCD_Driver.c Drivers/SPI1_Driver.c Drivers/SPI_Driver.c Drivers/SYSTEM_Driver.c Drivers/UART_Driver.c configuration.c main.c utils.c FSM_Controller.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP256GP502
MP_LINKER_FILE_OPTION=,--script=p33EP256GP502.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Controllers/MCP_Controller.o: Controllers/MCP_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/MCP_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/MCP_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/MCP_Controller.c  -o ${OBJECTDIR}/Controllers/MCP_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/MCP_Controller.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/MCP_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Controllers/MENU_Controller.o: Controllers/MENU_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/MENU_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/MENU_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/MENU_Controller.c  -o ${OBJECTDIR}/Controllers/MENU_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/MENU_Controller.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/MENU_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Controllers/SUPPLIES_Controller.o: Controllers/SUPPLIES_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/SUPPLIES_Controller.c  -o ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/SUPPLIES_Controller.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/SUPPLIES_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Controllers/UART_Controller.o: Controllers/UART_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/UART_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/UART_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/UART_Controller.c  -o ${OBJECTDIR}/Controllers/UART_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/UART_Controller.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/UART_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/ENC_Driver.o: Drivers/ENC_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/ENC_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/ENC_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/ENC_Driver.c  -o ${OBJECTDIR}/Drivers/ENC_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/ENC_Driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/ENC_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/I2C_Driver.o: Drivers/I2C_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/I2C_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/I2C_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/I2C_Driver.c  -o ${OBJECTDIR}/Drivers/I2C_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/I2C_Driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/I2C_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/LCD_Driver.o: Drivers/LCD_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/LCD_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/LCD_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/LCD_Driver.c  -o ${OBJECTDIR}/Drivers/LCD_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/LCD_Driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/LCD_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/SPI1_Driver.o: Drivers/SPI1_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/SPI1_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/SPI1_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/SPI1_Driver.c  -o ${OBJECTDIR}/Drivers/SPI1_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/SPI1_Driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/SPI1_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/SPI_Driver.o: Drivers/SPI_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/SPI_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/SPI_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/SPI_Driver.c  -o ${OBJECTDIR}/Drivers/SPI_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/SPI_Driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/SPI_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/SYSTEM_Driver.o: Drivers/SYSTEM_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/SYSTEM_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/SYSTEM_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/SYSTEM_Driver.c  -o ${OBJECTDIR}/Drivers/SYSTEM_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/SYSTEM_Driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/SYSTEM_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/UART_Driver.o: Drivers/UART_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/UART_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/UART_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/UART_Driver.c  -o ${OBJECTDIR}/Drivers/UART_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/UART_Driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/UART_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/configuration.o: configuration.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/configuration.o.d 
	@${RM} ${OBJECTDIR}/configuration.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  configuration.c  -o ${OBJECTDIR}/configuration.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/configuration.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/configuration.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/utils.o.d 
	@${RM} ${OBJECTDIR}/utils.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  utils.c  -o ${OBJECTDIR}/utils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/utils.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/utils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FSM_Controller.o: FSM_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/FSM_Controller.o.d 
	@${RM} ${OBJECTDIR}/FSM_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FSM_Controller.c  -o ${OBJECTDIR}/FSM_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FSM_Controller.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FSM_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/Controllers/MCP_Controller.o: Controllers/MCP_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/MCP_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/MCP_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/MCP_Controller.c  -o ${OBJECTDIR}/Controllers/MCP_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/MCP_Controller.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/MCP_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Controllers/MENU_Controller.o: Controllers/MENU_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/MENU_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/MENU_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/MENU_Controller.c  -o ${OBJECTDIR}/Controllers/MENU_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/MENU_Controller.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/MENU_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Controllers/SUPPLIES_Controller.o: Controllers/SUPPLIES_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/SUPPLIES_Controller.c  -o ${OBJECTDIR}/Controllers/SUPPLIES_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/SUPPLIES_Controller.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/SUPPLIES_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Controllers/UART_Controller.o: Controllers/UART_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Controllers" 
	@${RM} ${OBJECTDIR}/Controllers/UART_Controller.o.d 
	@${RM} ${OBJECTDIR}/Controllers/UART_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Controllers/UART_Controller.c  -o ${OBJECTDIR}/Controllers/UART_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Controllers/UART_Controller.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Controllers/UART_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/ENC_Driver.o: Drivers/ENC_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/ENC_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/ENC_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/ENC_Driver.c  -o ${OBJECTDIR}/Drivers/ENC_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/ENC_Driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/ENC_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/I2C_Driver.o: Drivers/I2C_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/I2C_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/I2C_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/I2C_Driver.c  -o ${OBJECTDIR}/Drivers/I2C_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/I2C_Driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/I2C_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/LCD_Driver.o: Drivers/LCD_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/LCD_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/LCD_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/LCD_Driver.c  -o ${OBJECTDIR}/Drivers/LCD_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/LCD_Driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/LCD_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/SPI1_Driver.o: Drivers/SPI1_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/SPI1_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/SPI1_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/SPI1_Driver.c  -o ${OBJECTDIR}/Drivers/SPI1_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/SPI1_Driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/SPI1_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/SPI_Driver.o: Drivers/SPI_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/SPI_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/SPI_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/SPI_Driver.c  -o ${OBJECTDIR}/Drivers/SPI_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/SPI_Driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/SPI_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/SYSTEM_Driver.o: Drivers/SYSTEM_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/SYSTEM_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/SYSTEM_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/SYSTEM_Driver.c  -o ${OBJECTDIR}/Drivers/SYSTEM_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/SYSTEM_Driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/SYSTEM_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Drivers/UART_Driver.o: Drivers/UART_Driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drivers" 
	@${RM} ${OBJECTDIR}/Drivers/UART_Driver.o.d 
	@${RM} ${OBJECTDIR}/Drivers/UART_Driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Drivers/UART_Driver.c  -o ${OBJECTDIR}/Drivers/UART_Driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Drivers/UART_Driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Drivers/UART_Driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/configuration.o: configuration.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/configuration.o.d 
	@${RM} ${OBJECTDIR}/configuration.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  configuration.c  -o ${OBJECTDIR}/configuration.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/configuration.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/configuration.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/utils.o.d 
	@${RM} ${OBJECTDIR}/utils.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  utils.c  -o ${OBJECTDIR}/utils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/utils.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/utils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FSM_Controller.o: FSM_Controller.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/FSM_Controller.o.d 
	@${RM} ${OBJECTDIR}/FSM_Controller.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FSM_Controller.c  -o ${OBJECTDIR}/FSM_Controller.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FSM_Controller.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FSM_Controller.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/MainController.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
