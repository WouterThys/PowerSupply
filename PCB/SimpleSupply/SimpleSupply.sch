EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:MyLib
LIBS:SimpleSupply-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LT3080 U1
U 1 1 5A6DEF97
P 3850 2250
F 0 "U1" H 4100 2100 60  0000 C CNN
F 1 "LT3080" H 3850 2600 60  0000 C CNN
F 2 "TO_SOT_Packages_THT:TO-220-5_Pentawatt_Multiwatt-5_Vertical_StaggeredType1" H 3850 2250 60  0001 C CNN
F 3 "" H 3850 2250 60  0001 C CNN
	1    3850 2250
	1    0    0    -1  
$EndComp
$Comp
L Barrel_Jack J1
U 1 1 5A6DF006
P 1600 2150
F 0 "J1" H 1600 2360 50  0000 C CNN
F 1 "Barrel_Jack" H 1600 1975 50  0000 C CNN
F 2 "Connectors:BARREL_JACK" H 1650 2110 50  0001 C CNN
F 3 "" H 1650 2110 50  0001 C CNN
	1    1600 2150
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 5A6DF11D
P 2900 2400
F 0 "C2" H 2925 2500 50  0000 L CNN
F 1 "100n" H 2925 2300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 2938 2250 50  0001 C CNN
F 3 "" H 2900 2400 50  0001 C CNN
	1    2900 2400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5A6DF171
P 2900 2650
F 0 "#PWR02" H 2900 2400 50  0001 C CNN
F 1 "GND" H 2900 2500 50  0000 C CNN
F 2 "" H 2900 2650 50  0001 C CNN
F 3 "" H 2900 2650 50  0001 C CNN
	1    2900 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 2650 2900 2550
Wire Wire Line
	1900 2050 3350 2050
Wire Wire Line
	2900 950  2900 2250
Connection ~ 2900 2050
$Comp
L GND #PWR03
U 1 1 5A6DF1FF
P 2000 2650
F 0 "#PWR03" H 2000 2400 50  0001 C CNN
F 1 "GND" H 2000 2500 50  0000 C CNN
F 2 "" H 2000 2650 50  0001 C CNN
F 3 "" H 2000 2650 50  0001 C CNN
	1    2000 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 2650 2000 2250
Wire Wire Line
	2000 2250 1900 2250
NoConn ~ 1900 2150
$Comp
L R R2
U 1 1 5A6DF267
P 3950 2850
F 0 "R2" V 4030 2850 50  0000 C CNN
F 1 "499k" V 3950 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 3880 2850 50  0001 C CNN
F 3 "" H 3950 2850 50  0001 C CNN
	1    3950 2850
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 5A6DF2CC
P 4200 2850
F 0 "C3" H 4225 2950 50  0000 L CNN
F 1 "100n" H 4225 2750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4238 2700 50  0001 C CNN
F 3 "" H 4200 2850 50  0001 C CNN
	1    4200 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 2550 3950 2700
Wire Wire Line
	4200 2700 4200 2650
Wire Wire Line
	4200 2650 3950 2650
Connection ~ 3950 2650
$Comp
L GND #PWR04
U 1 1 5A6DF371
P 3950 3100
F 0 "#PWR04" H 3950 2850 50  0001 C CNN
F 1 "GND" H 3950 2950 50  0000 C CNN
F 2 "" H 3950 3100 50  0001 C CNN
F 3 "" H 3950 3100 50  0001 C CNN
	1    3950 3100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 5A6DF391
P 4200 3100
F 0 "#PWR05" H 4200 2850 50  0001 C CNN
F 1 "GND" H 4200 2950 50  0000 C CNN
F 2 "" H 4200 3100 50  0001 C CNN
F 3 "" H 4200 3100 50  0001 C CNN
	1    4200 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3100 4200 3000
Wire Wire Line
	3950 3000 3950 3100
$Comp
L LT3080 U2
U 1 1 5A6DF74A
P 5800 2250
F 0 "U2" H 6050 2100 60  0000 C CNN
F 1 "LT3080" H 5800 2600 60  0000 C CNN
F 2 "TO_SOT_Packages_THT:TO-220-5_Pentawatt_Multiwatt-5_Vertical_StaggeredType1" H 5800 2250 60  0001 C CNN
F 3 "" H 5800 2250 60  0001 C CNN
	1    5800 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 2050 5300 2050
Wire Wire Line
	5300 2150 5200 2150
Wire Wire Line
	5200 2050 5200 2300
Connection ~ 5200 2050
$Comp
L R R4
U 1 1 5A6DF754
P 5900 2850
F 0 "R4" V 5980 2850 50  0000 C CNN
F 1 "330k" V 5900 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 5830 2850 50  0001 C CNN
F 3 "" H 5900 2850 50  0001 C CNN
	1    5900 2850
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5A6DF75A
P 6150 2850
F 0 "C6" H 6175 2950 50  0000 L CNN
F 1 "100n" H 6175 2750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 6188 2700 50  0001 C CNN
F 3 "" H 6150 2850 50  0001 C CNN
	1    6150 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 2550 5900 2700
Wire Wire Line
	6150 2700 6150 2650
Wire Wire Line
	6150 2650 5900 2650
Connection ~ 5900 2650
$Comp
L GND #PWR06
U 1 1 5A6DF764
P 5900 3100
F 0 "#PWR06" H 5900 2850 50  0001 C CNN
F 1 "GND" H 5900 2950 50  0000 C CNN
F 2 "" H 5900 3100 50  0001 C CNN
F 3 "" H 5900 3100 50  0001 C CNN
	1    5900 3100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 5A6DF76A
P 6150 3100
F 0 "#PWR07" H 6150 2850 50  0001 C CNN
F 1 "GND" H 6150 2950 50  0000 C CNN
F 2 "" H 6150 3100 50  0001 C CNN
F 3 "" H 6150 3100 50  0001 C CNN
	1    6150 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 3100 6150 3000
Wire Wire Line
	5900 3000 5900 3100
$Comp
L CP1 C4
U 1 1 5A6DF7AD
P 4650 2350
F 0 "C4" H 4675 2450 50  0000 L CNN
F 1 "10µ" H 4675 2250 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:CP_Tantalum_Case-A_EIA-3216-18_Hand" H 4650 2350 50  0001 C CNN
F 3 "" H 4650 2350 50  0001 C CNN
	1    4650 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 5A6DF818
P 4650 2600
F 0 "#PWR08" H 4650 2350 50  0001 C CNN
F 1 "GND" H 4650 2450 50  0000 C CNN
F 2 "" H 4650 2600 50  0001 C CNN
F 3 "" H 4650 2600 50  0001 C CNN
	1    4650 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 2600 4650 2500
Wire Wire Line
	6300 2050 7350 2050
$Comp
L CP1 C7
U 1 1 5A6DF959
P 6700 2350
F 0 "C7" H 6725 2450 50  0000 L CNN
F 1 "10µ" H 6725 2250 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:CP_Tantalum_Case-A_EIA-3216-18_Hand" H 6700 2350 50  0001 C CNN
F 3 "" H 6700 2350 50  0001 C CNN
	1    6700 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 5A6DF9BB
P 6700 2600
F 0 "#PWR09" H 6700 2350 50  0001 C CNN
F 1 "GND" H 6700 2450 50  0000 C CNN
F 2 "" H 6700 2600 50  0001 C CNN
F 3 "" H 6700 2600 50  0001 C CNN
	1    6700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 2600 6700 2500
Wire Wire Line
	6700 2200 6700 2050
Connection ~ 6700 2050
Text Label 2700 2050 0    60   ~ 0
Vin
Text Label 4600 2050 0    60   ~ 0
5V
Text Label 6450 2050 0    60   ~ 0
3.3V
$Comp
L Conn_01x02 J2
U 1 1 5A72158F
P 3400 950
F 0 "J2" H 3400 1050 50  0000 C CNN
F 1 "Vin_2" H 3400 750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 3400 950 50  0001 C CNN
F 3 "" H 3400 950 50  0001 C CNN
	1    3400 950 
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02 J4
U 1 1 5A721655
P 5200 950
F 0 "J4" H 5200 1050 50  0000 C CNN
F 1 "5V_2" H 5200 750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 5200 950 50  0001 C CNN
F 3 "" H 5200 950 50  0001 C CNN
	1    5200 950 
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02 J6
U 1 1 5A7216B9
P 7400 950
F 0 "J6" H 7400 1050 50  0000 C CNN
F 1 "3V3_2" H 7400 750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 7400 950 50  0001 C CNN
F 3 "" H 7400 950 50  0001 C CNN
	1    7400 950 
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x04 J3
U 1 1 5A7217D0
P 3400 1450
F 0 "J3" H 3400 1650 50  0000 C CNN
F 1 "Vin_4" H 3400 1150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch1.27mm" H 3400 1450 50  0001 C CNN
F 3 "" H 3400 1450 50  0001 C CNN
	1    3400 1450
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x04 J5
U 1 1 5A721942
P 5200 1450
F 0 "J5" H 5200 1650 50  0000 C CNN
F 1 "5V_4" H 5200 1150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch1.27mm" H 5200 1450 50  0001 C CNN
F 3 "" H 5200 1450 50  0001 C CNN
	1    5200 1450
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x04 J7
U 1 1 5A7219D0
P 7400 1450
F 0 "J7" H 7400 1650 50  0000 C CNN
F 1 "3V3_4" H 7400 1150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch1.27mm" H 7400 1450 50  0001 C CNN
F 3 "" H 7400 1450 50  0001 C CNN
	1    7400 1450
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 5A721AD1
P 4900 2350
F 0 "C5" H 4925 2450 50  0000 L CNN
F 1 "10n" H 4925 2250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4938 2200 50  0001 C CNN
F 3 "" H 4900 2350 50  0001 C CNN
	1    4900 2350
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 5A721B3E
P 6950 2350
F 0 "C8" H 6975 2450 50  0000 L CNN
F 1 "10n" H 6975 2250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 6988 2200 50  0001 C CNN
F 3 "" H 6950 2350 50  0001 C CNN
	1    6950 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 5A721C36
P 4900 2600
F 0 "#PWR010" H 4900 2350 50  0001 C CNN
F 1 "GND" H 4900 2450 50  0000 C CNN
F 2 "" H 4900 2600 50  0001 C CNN
F 3 "" H 4900 2600 50  0001 C CNN
	1    4900 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR011
U 1 1 5A721C7D
P 6950 2600
F 0 "#PWR011" H 6950 2350 50  0001 C CNN
F 1 "GND" H 6950 2450 50  0000 C CNN
F 2 "" H 6950 2600 50  0001 C CNN
F 3 "" H 6950 2600 50  0001 C CNN
	1    6950 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 2600 6950 2500
Wire Wire Line
	6950 950  6950 2200
Wire Wire Line
	4900 2500 4900 2600
Wire Wire Line
	3200 950  2900 950 
Wire Wire Line
	3200 1450 2900 1450
Connection ~ 2900 1450
Wire Wire Line
	3200 1350 2900 1350
Connection ~ 2900 1350
Wire Wire Line
	4650 950  5000 950 
Wire Wire Line
	4650 1350 5000 1350
Wire Wire Line
	4650 1450 5000 1450
Connection ~ 6950 2050
Wire Wire Line
	6950 950  7200 950 
Wire Wire Line
	7200 1350 6950 1350
Connection ~ 6950 1350
Wire Wire Line
	7200 1450 6950 1450
Connection ~ 6950 1450
$Comp
L LED D3
U 1 1 5A72275C
P 7350 2400
F 0 "D3" H 7350 2500 50  0000 C CNN
F 1 "LED" H 7350 2300 50  0000 C CNN
F 2 "LEDs:LED_0603_HandSoldering" H 7350 2400 50  0001 C CNN
F 3 "" H 7350 2400 50  0001 C CNN
	1    7350 2400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3250 2050 3250 2250
Connection ~ 3250 2050
Wire Wire Line
	3350 2150 3250 2150
Connection ~ 3250 2150
$Comp
L GND #PWR012
U 1 1 5A722966
P 7350 3100
F 0 "#PWR012" H 7350 2850 50  0001 C CNN
F 1 "GND" H 7350 2950 50  0000 C CNN
F 2 "" H 7350 3100 50  0001 C CNN
F 3 "" H 7350 3100 50  0001 C CNN
	1    7350 3100
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 5A722A88
P 7350 2850
F 0 "R5" V 7430 2850 50  0000 C CNN
F 1 "220" V 7350 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 7280 2850 50  0001 C CNN
F 3 "" H 7350 2850 50  0001 C CNN
	1    7350 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 2700 7350 2550
Wire Wire Line
	7350 3000 7350 3100
Connection ~ 5200 2150
Wire Wire Line
	4650 950  4650 2200
Connection ~ 4650 2050
Wire Wire Line
	4900 2050 4900 2200
Connection ~ 4900 2050
Connection ~ 4650 1350
Connection ~ 4650 1450
$Comp
L LED D1
U 1 1 5A7237BC
P 3250 2400
F 0 "D1" H 3250 2500 50  0000 C CNN
F 1 "LED" H 3250 2300 50  0000 C CNN
F 2 "LEDs:LED_0603_HandSoldering" H 3250 2400 50  0001 C CNN
F 3 "" H 3250 2400 50  0001 C CNN
	1    3250 2400
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR013
U 1 1 5A7237C2
P 3250 3100
F 0 "#PWR013" H 3250 2850 50  0001 C CNN
F 1 "GND" H 3250 2950 50  0000 C CNN
F 2 "" H 3250 3100 50  0001 C CNN
F 3 "" H 3250 3100 50  0001 C CNN
	1    3250 3100
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 5A7237C8
P 3250 2850
F 0 "R1" V 3330 2850 50  0000 C CNN
F 1 "1k" V 3250 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 3180 2850 50  0001 C CNN
F 3 "" H 3250 2850 50  0001 C CNN
	1    3250 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 2700 3250 2550
Wire Wire Line
	3250 3000 3250 3100
Wire Wire Line
	7350 2050 7350 2250
$Comp
L LED D2
U 1 1 5A723A89
P 5200 2450
F 0 "D2" H 5200 2550 50  0000 C CNN
F 1 "LED" H 5200 2350 50  0000 C CNN
F 2 "LEDs:LED_0603_HandSoldering" H 5200 2450 50  0001 C CNN
F 3 "" H 5200 2450 50  0001 C CNN
	1    5200 2450
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR014
U 1 1 5A723A8F
P 5200 3150
F 0 "#PWR014" H 5200 2900 50  0001 C CNN
F 1 "GND" H 5200 3000 50  0000 C CNN
F 2 "" H 5200 3150 50  0001 C CNN
F 3 "" H 5200 3150 50  0001 C CNN
	1    5200 3150
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 5A723A95
P 5200 2900
F 0 "R3" V 5280 2900 50  0000 C CNN
F 1 "390" V 5200 2900 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 5130 2900 50  0001 C CNN
F 3 "" H 5200 2900 50  0001 C CNN
	1    5200 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 2750 5200 2600
Wire Wire Line
	5200 3050 5200 3150
$Comp
L GND #PWR015
U 1 1 5A7247CD
P 3100 1750
F 0 "#PWR015" H 3100 1500 50  0001 C CNN
F 1 "GND" H 3100 1600 50  0000 C CNN
F 2 "" H 3100 1750 50  0001 C CNN
F 3 "" H 3100 1750 50  0001 C CNN
	1    3100 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 1050 3100 1750
Wire Wire Line
	3100 1050 3200 1050
Wire Wire Line
	3200 1550 3100 1550
Connection ~ 3100 1550
Wire Wire Line
	3200 1650 3100 1650
Connection ~ 3100 1650
$Comp
L GND #PWR016
U 1 1 5A7249B6
P 4900 1750
F 0 "#PWR016" H 4900 1500 50  0001 C CNN
F 1 "GND" H 4900 1600 50  0000 C CNN
F 2 "" H 4900 1750 50  0001 C CNN
F 3 "" H 4900 1750 50  0001 C CNN
	1    4900 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 1050 4900 1750
Wire Wire Line
	4900 1050 5000 1050
Wire Wire Line
	5000 1550 4900 1550
Connection ~ 4900 1550
Wire Wire Line
	5000 1650 4900 1650
Connection ~ 4900 1650
$Comp
L GND #PWR017
U 1 1 5A724BFF
P 7100 1800
F 0 "#PWR017" H 7100 1550 50  0001 C CNN
F 1 "GND" H 7100 1650 50  0000 C CNN
F 2 "" H 7100 1800 50  0001 C CNN
F 3 "" H 7100 1800 50  0001 C CNN
	1    7100 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 1050 7100 1800
Wire Wire Line
	7100 1050 7200 1050
Wire Wire Line
	7200 1550 7100 1550
Connection ~ 7100 1550
Wire Wire Line
	7200 1650 7100 1650
Connection ~ 7100 1650
$Comp
L HEATSINK HS1
U 1 1 5A725138
P 3900 1650
F 0 "HS1" H 3900 1850 50  0000 C CNN
F 1 "HEATSINK" H 3900 1600 50  0000 C CNN
F 2 "KiCad:Heatsink_Fischer_SK104-STCB_35x13mm__2xDrill3.5mm_ScrewM3" H 3900 1650 50  0001 C CNN
F 3 "" H 3900 1650 50  0000 C CNN
	1    3900 1650
	1    0    0    -1  
$EndComp
$Comp
L HEATSINK HS2
U 1 1 5A725226
P 5800 1600
F 0 "HS2" H 5800 1800 50  0000 C CNN
F 1 "HEATSINK" H 5800 1550 50  0000 C CNN
F 2 "KiCad:Heatsink_Fischer_SK104-STCB_35x13mm__2xDrill3.5mm_ScrewM3" H 5800 1600 50  0001 C CNN
F 3 "" H 5800 1600 50  0000 C CNN
	1    5800 1600
	1    0    0    -1  
$EndComp
$EndSCHEMATC