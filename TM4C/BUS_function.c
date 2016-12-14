


#include "SYSTEM.h"

extern int DeviceCounter ;
extern int Devices[2][7];

void Init_Bus( void ) {

	SYSCTL_RCGCGPIO_R = 0x00000080;				//PortN Clock einschalten

	GPIO_PORTH_AHB_DIR_R = 0x01;
	GPIO_PORTH_AHB_DATA_R = 0x01;
	GPIO_PORTH_AHB_DEN_R = 0x03;

}

boolean Bus_WriteByte( int Data ) {

	int i = 0;
	int out;

	for( i = 0; i < 8; i ++ ) {
		out = Bus_WriteBit( (Data >> i) & 0x01 );
		if( out == False )
			return False;
	}
	return True;
}

int Bus_ReadByte( void ) {

	int i = 0;
	int Data = 0;
	for( i = 0; i < 8; i++ ) {
		Data = Data | ( Bus_ReadBit() << (i) );
	}
	return Data;
}

boolean Bus_Reset( void ) {

	wait(BUS_H_G);

	GPIO_PORTH_AHB_DATA_R = GPIO_PORTH_AHB_DATA_R & ~ 0x01;

	wait(BUS_H_H);

	GPIO_PORTH_AHB_DATA_R = GPIO_PORTH_AHB_DATA_R | 0x01;

	wait(BUS_H_I);

	if( GPIO_PORTH_AHB_DATA_R & 0x02 ) {
		return False;
	}
	wait(BUS_H_J);
	return True;

}

boolean Bus_WriteBit( int Output ) {
	if(Output == 1) {

		GPIO_PORTH_AHB_DATA_R = (GPIO_PORTH_AHB_DATA_R & ~0x01);

		wait(BUS_H_A/2);

		if( GPIO_PORTH_AHB_DATA_R & 0x02 ) {

			GPIO_PORTH_AHB_DATA_R = (GPIO_PORTH_AHB_DATA_R | 0x01);
			return False;

		}

		wait(BUS_H_A/2);

		GPIO_PORTH_AHB_DATA_R = (GPIO_PORTH_AHB_DATA_R | 0x01);

		wait(BUS_H_B/2);

		if( ~GPIO_PORTH_AHB_DATA_R & 0x02 ) {

			return False;

		}

		wait(BUS_H_B/2);


	} else if ( Output == 0) {

		GPIO_PORTH_AHB_DATA_R = (GPIO_PORTH_AHB_DATA_R & ~0x01);

		wait(BUS_H_C/2);

		if( GPIO_PORTH_AHB_DATA_R & 0x02 ) {

			GPIO_PORTH_AHB_DATA_R = (GPIO_PORTH_AHB_DATA_R | 0x01);
			return False;

		}

		wait(BUS_H_C/2);
		GPIO_PORTH_AHB_DATA_R = (GPIO_PORTH_AHB_DATA_R | 0x01);

		wait(BUS_H_D);

	}

	return True;
}

int Bus_ReadBit( void ) {

	int Input = 0;

	GPIO_PORTH_AHB_DATA_R = GPIO_PORTH_AHB_DATA_R & ~0x01;

	wait(BUS_H_A);

	GPIO_PORTH_AHB_DATA_R = GPIO_PORTH_AHB_DATA_R | 0x01;

	wait(BUS_H_E);

	Input = (GPIO_PORTH_AHB_DATA_R & 0x02) >> 1;

	wait(BUS_H_F);

	return Input;
}

boolean init_OneDevice( void ) {
	boolean Reset;

	Reset = Bus_Reset();	// Reset

	if (Reset == False ) {
		return False;
	}
	Bus_WriteByte(0xCC);	// Skip Rom / Ansprechen aller Sensoren auf dem Bus
	Bus_WriteByte(0x4E);	// Write Scratchpad /

	Bus_WriteByte(0x07);	// TH Alram Value
	Bus_WriteByte(0xD0);	// TL
	Bus_WriteByte(0x60);	// Write Configuration Register 12 Bit Resolution

	Reset = Bus_Reset();	// Reset

	if (Reset == False ) {
		return False;
	} else {
		return True;
	}

}

double Read_OneDevice( void ) {

	int temp, Data, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8 = 0;
	double temperatur = 0;
	boolean Reset;

	Bus_WriteByte(0xCC);
	Bus_WriteByte(0x44);

	Reset = Bus_Reset();	// Reset

	if( Reset == False ) {
		return 0xFFFFFFFF;
	}

	Bus_WriteByte(0xCC);	// Skip ROm / Ansprechen aller Sensoren auf dem Bus
	Bus_WriteByte(0xBE);	// Read Scratchpad

	Data  = Bus_ReadByte();
	Data1 = Bus_ReadByte();
	Data2 = Bus_ReadByte();
	Data3 = Bus_ReadByte();
	Data4 = Bus_ReadByte();
	Data5 = Bus_ReadByte();
	Data6 = Bus_ReadByte();
	Data7 = Bus_ReadByte();
	Data8 = Bus_ReadByte();

	Reset = Bus_Reset();	// Reset

	if( Reset == False ) {
		return 0xFFFFFFFF;
	}

	temp = Data | (Data1 << 8);

	if ( temp & 0xF800 ) {
		temp = temp & ~0xF800;
		temp = temp * (-1);
	}
	temperatur = (double)temp;
	temperatur = temperatur * 0.0625;

	return temperatur;
}

boolean Init_MultipleDevices( void ) {
	int i = 0, k = 0, j = 0, g = 0, temp1, temp2, notequal = 0, notequalold = 0, status = 0, between = 0, betweenFlag = 0;
	int bit_double[5] = {{0}};
	extern int DeviceCounter;
	extern int Devices[2][7];
	boolean Reset;

	Devices[0][6] = 0xFFFFFFFF;
	Devices[1][6] = 0xFFFFFFFF;
	do {
		notequalold = notequal;
		notequal = 0;
		g = 0;

		Reset = Bus_Reset();

		if ( Reset == False ) {
			return Reset;
		}

		Bus_WriteByte(0xF0);

		for (i = 0; i < 2; i++) {

			for (j = 0; j < 32; j++) {
				temp1 = Bus_ReadBit();
				temp2 = Bus_ReadBit();

				if(temp1 == temp2 && k != 0) {
					if(k == 1 && notequal == 0) {
						printf("%i\n",j);
						bit_double[0] = j;
						Bus_WriteBit(0x01);
						Devices[i][k] = Devices[i][k] << 1;
						Devices[i][k] |= 0x01;
						notequal++;
					} else if( k == 2 && notequal < 2) {
						printf("%i\n",j);
						bit_double[1] = j;
						Bus_WriteBit(0x01);
						Devices[i][k] = Devices[i][k] << 1;
						Devices[i][k] |= 0x01;
						notequal++;
					} else if(k == 3 && notequal < 3) {
						printf("%i\n",j);
						bit_double[2] = j;
						Bus_WriteBit(0x01);
						Devices[i][k] = Devices[i][k] << 1;
						Devices[i][k] |= 0x01;
						notequal++;
					} else if( k == 4 && notequal < 4) {
						printf("%i\n",j);
						bit_double[3] = j;
						Bus_WriteBit(0x01);
						Devices[i][k] = Devices[i][k] << 1;
						Devices[i][k] |= 0x01;
						notequal++;
					} else if( k == 5 && notequal < 5) {
						printf("%i\n",j);
						bit_double[4] = j;
						Bus_WriteBit(0x01);
						Devices[i][k] = Devices[i][k] << 1;
						Devices[i][k] |= 0x01;
						notequal++;
					} else {
						Bus_WriteBit(temp1);
						Devices[i][k] = Devices[i][k] << 1;
						Devices[i][k] |= temp1;
					}
				} else {
					Bus_WriteBit(temp1);
					Devices[i][k] = Devices[i][k] << 1;
					Devices[i][k] |= temp1;
				}

			}
		}
		k++;

	} while( k == 1 || notequal != notequalold );

	DeviceCounter = k-2;

	if (DeviceCounter == 1) {
		return False;
	}

	for( k = 0; k < 2; k++ ) {

		Reset = Bus_Reset();	// Reset
		Bus_WriteByte(0x55);	// MATCH Rom / Ansprechen eines Sensors auf dem Bus

		for( i = 0; i < 2; i++ ) {

			for (j = 0; j < 32; j++) {

				temp1 = ( Devices[i][k] ) >> (31 - j) & 0x00000001;
				Bus_WriteBit(temp1);

			}
		}

		Bus_WriteByte(0x4E);	// Write Scratchpad /

		Bus_WriteByte(0x07);	// TH Alram Value
		Bus_WriteByte(0xD0);	// TL
		Bus_WriteByte(0x60);	// Write Configuration Register 12 Bit Resolution

		Reset = Bus_Reset();	// Reset

	}

	return True;

}

double Read_Device( int Dev ) {
	int i = 0, j = 0, Data, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Reset, temp1, temp;
	double temperatur = 0;
	extern int DeviceCounter;
	extern int Devices[][];

	Bus_WriteByte(0xCC); 	// Skip Rom Comand
	Bus_WriteByte(0x44); 	// Start Convertion

	wait(10);

	Reset = Bus_Reset();	// Reset
	Bus_WriteByte(0x55);	// MATCH Rom / Ansprechen eines Sensors auf dem Bus

	for( i = 0; i < 2; i++ ) {

		for (j = 0; j < 32; j++) {

			temp1 = ( Devices[i][Dev] ) >> (31 - j) & 0x00000001;
			Bus_WriteBit(temp1);

		}
	}

	Bus_WriteByte(0xBE);	// Read Scratchpad

	Data  = Bus_ReadByte();
	Data1 = Bus_ReadByte();
	Data2 = Bus_ReadByte();
	Data3 = Bus_ReadByte();
	Data4 = Bus_ReadByte();
	Data5 = Bus_ReadByte();
	Data6 = Bus_ReadByte();
	Data7 = Bus_ReadByte();
	Data8 = Bus_ReadByte();

	Reset = Bus_Reset();	// Reset

	if( Reset == False ) {
		return 0xFFFFFFFF;
	}

	CRC_Check( Data, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8);

	temp = Data | (Data1 << 8);

	if ( temp & 0xF800 ) {
		temp = temp & ~0xF800;
		temp = temp * (-1);
	}
	temperatur = (double)temp;
	temperatur = temperatur * 0.0625;

	return temperatur;

}

int CRC_Check( int Data0, int Data1, int Data2, int Data3, int Data4, int Data5, int Data6, int Data7, int Data8 ) {
	int i, k;
	int polynom = 0x119, bit_in = 0, temp = 0, speicher = 0;

	Data0 = (Data0 << 24 ) | (Data1 << 16) | (Data2 << 8) | (Data3);

	for(k = 0; k < 2; k++) {								// Einschub der ersten 8 Bytes in den Generator

		for( i = 31; i >= 0; i-- ) {

			bit_in = (Data0 >> i) & 0x01;

			temp = ((speicher & 0x01) ^ bit_in) * polynom;
			temp = temp >> 1;

			speicher = speicher >> 1;
			speicher = speicher ^ temp;
		}
		Data0 = (Data4 << 24 ) | (Data5 << 16) | (Data6 << 8) | (Data7);
	}

	for( i = 7; i >= 0; i-- ) {								// Einschub des letzten Bytes in den Generator

		bit_in = (Data8 >> i) & 0x01;

		temp = ((speicher & 0x01) ^ bit_in) * polynom;
		temp = temp >> 1;

		speicher = speicher >> 1;
		speicher = speicher ^ temp;
	}

	return speicher;									 	// Rückgabe des Schieberegisterinhalts

}





























