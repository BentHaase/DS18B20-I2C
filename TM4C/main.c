/*
 * main.c
 *
 * PN4: Output	PN5: Input
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "inc/tm4c1294ncpdt.h"
#include "BUS.h"
#include "LCD.h"

void wait(int Time);

int DeviceCounter;
int Devices[2][5];





int main(void) {

	float temperatur = 0;
	int i, j = 0;
	boolean success, success_old, oneDevice;

	Init_Bus();
	Init();
	InitDisplay();

	display_pic();
	wait(3000000);

	display_standart();
	success = Init_MultipleDevices( );

	while(1) {

		success_old = success;

		if ( success == False ) {
			init_OneDevice( );
			temperatur = Read_OneDevice( );
			display_plot_temperature(temperatur, 0);
		} else {
			for( i = 0; i < DeviceCounter ; i++ ) { //DeviceCounter
				temperatur = Read_Device(i);
				display_plot_temperature(temperatur, i);
			}
		}

		j++;

		success = Init_MultipleDevices( );

		if( j == 3000 || success_old != success ) {
			display_standart();
			j = 0;
		}
	}
}



void wait(int Time) {
	volatile uint32_t ui32Loop;				// Speicher im CPU Register für Schleife
	for(ui32Loop = 0; ui32Loop < Time; ui32Loop++)
	{
	}
}


