/*
 * BUS.h
 *
 *  Created on: 20.10.2016
 *      Author: Sebastian
 */

#define boolean int
#define True	1
#define False	0

#define BUS_H_A	6 * 1.6
#define BUS_H_B	64 * 1.6
#define BUS_H_C	60 * 1.6
#define BUS_H_D	10 * 1.6
#define BUS_H_E	9 * 1.6
#define BUS_H_F	55 * 1.6
#define BUS_H_G	0 * 1.6
#define BUS_H_H	480 * 1.6
#define BUS_H_I	70 * 1.6
#define BUS_H_J	410 * 3//1.6

void Init_Bus( void );
boolean Bus_Reset( void );
boolean Bus_WriteBit(int Output);
int Bus_ReadBit( void );
boolean Bus_WriteByte( int Data );
int Bus_ReadByte( void );
boolean init_OneDevice( void );
double Read_OneDevice( void );
boolean Init_MultipleDevices( void );
double Read_Device( int Dev );




