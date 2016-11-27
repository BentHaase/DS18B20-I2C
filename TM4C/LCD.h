/*
 * LCD.h
 *
 *  Created on: 19.10.2016
 *      Author: Sebastian
 */
#include "BUS.h"

#define LCD_WAIT	    0

//#define RAMWR		0x0000025C
//#define WRMASK		0x00000600
//#define DISON		0x00000CAF

//#define DISCTRL 	0x00000CCA
#define DISCTRL_PB1	0x00000D00
#define DISCTRL_PB2	0x00000D27
#define DISCTRL_PB3	0x00000D07

// Display Größe
#define DISLENGTH	240
#define DISHIGH		160
#define OFFSET		16		// Offset der Nulllinie der Maske

// Dispay Befehle
#define DISON 0xAF
#define DISOFF 0xAE
#define DISNOR 0xA6
#define DISINV 0xA7
#define COMSCN 0xBB
#define DISCTRL 0xCA
#define SLPIN 0x95
#define SLPOUT 0x94
#define LASET 0x75
#define CASET 0x15
#define DATSDR 0xBC
#define RAMWR 0x5C
#define RAMRD 0x5D
#define PTLIN 0xA8
#define PTLOUT 0xA9
#define RMWIN 0xE0
#define RMWOUT 0xEE
#define ASCSET 0xAA
#define SCSTART 0xAB
#define OSCON 0xD1
#define OSCOFF 0xD2
#define PWRCTRL 0x20
#define VOLCTRL 0x81
#define VOLUP 0xD6
#define VOLDOWN 0xD7
#define EXTIN 0x30
#define EXTOUT 0x31
#define ANASET 0x32
#define SWINT 0x34


void Init( void );

void LCD_CmdWrite( int Output );
void LCD_DataWrite( int Output );				// Eingabeparamteter: ' /RD /WR RS /CS DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0'
int LCD_DataRead( void );
void LCD_ReadEEPROM( void );
void display_full(int dat);
void display_pic( void );
void display_standart( void );
void display_plot( int*Measure );
void display_gray( void );
void InitDisplay();
boolean display_plot_temperature( double temperatur, int Sensor );
