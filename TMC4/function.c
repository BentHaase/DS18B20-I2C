



#include "BUS.h"
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"


boolean Bus_WriteByte( int Data ) {

	int i = 0;
	int out;

	for( i = 0; i < 8; i ++ ) {
		out = Bus_WriteBit( (Data >> 7-i) & 0x01 );
		if( out == False )
			return False;
	}
	return True;
}

int Bus_ReadByte( void ) {

	int i = 0;
	int Data = 0;
	for( i = 0; i < 8; i++ ) {
		Data = Data | ( Bus_ReadBit() << (7-i) );
	}
	return Data;
}

boolean Bus_Reset( void ) {

	wait(BUS_H_G);

	GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R & ~ 0x10;

	wait(BUS_H_H);

	GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R | 0x10;

	wait(BUS_H_I);

	if( GPIO_PORTN_DATA_R & 0x20 ) {
		return False;
	}
	wait(BUS_H_J);
	return True;

}

boolean Bus_WriteBit( int Output ) {
	if(Output == 1) {

		GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R & ~0x10);

		wait(BUS_H_A/2);

		if( GPIO_PORTN_DATA_R & 0x20 ) {

			GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R | 0x10);
			return False;

		}

		wait(BUS_H_A/2);

		GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R | 0x10);

		wait(BUS_H_B/2);

		if( ~GPIO_PORTN_DATA_R & 0x20 ) {

			return False;

		}

		wait(BUS_H_B/2);


	} else if ( Output == 0) {

		GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R & ~0x10);

		wait(BUS_H_C/2);

		if( GPIO_PORTN_DATA_R & 0x20 ) {

			GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R | 0x10);
			return False;

		}

		wait(BUS_H_C/2);
		GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R | 0x10);

		wait(BUS_H_D);

	}

	return True;
}

int Bus_ReadBit( void ) {

	int Input = 0;

	GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R & ~0x10;

	wait(BUS_H_A);

	GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R | 0x10;

	wait(BUS_H_E);

	Input = (GPIO_PORTN_DATA_R & 0x20) >> 5;

	wait(BUS_H_F);

	return Input;
}


void wait(int Time) {
	volatile uint32_t ui32Loop;				// Speicher im CPU Register für Schleife
    for(ui32Loop = 0; ui32Loop < Time; ui32Loop++)
    {
    }
}
