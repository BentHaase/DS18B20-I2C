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

int main(void) {
	
	SYSCTL_RCGCGPIO_R = 0x00001000;				//PortN Clock einschalten

	GPIO_PORTN_DIR_R = 0x10;
	GPIO_PORTN_DATA_R = 0x10;
	GPIO_PORTN_DEN_R = 0x30;




	int Reset, Data, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8 = 0;



	while(1) {
		/*
		printf("1 auf Bus schreiben \n");
		Data = Bus_WriteBit(0x01);
		printf("%i\n", Data);

		wait(5000000);
		printf("0 auf Bus schreiben \n");
		Data = Bus_WriteBit(0x00);
		printf("%i\n", Data);

		wait(5000000);
		printf("Bus lesen \n");
		Data = Bus_ReadBit();
		printf("%i\n", Data);

		wait(5000000);
		printf("Bus reseten\n");
		Data = Bus_Reset();*/
		Reset = Bus_Reset();	// Reset

		Bus_WriteByte(0xCC);	// Skip Rom / Ansprechen aller Sensoren auf dem Bus
		Bus_WriteByte(0x4E);	// Write Scratchpad /

		Bus_WriteByte(0x33);	// 3 Data Bytes
		Bus_WriteByte(0x33);
		Bus_WriteByte(0x33);

		Reset = Bus_Reset();	// Reset

		Bus_WriteByte(0xCC);
		Bus_WriteByte(0xBE);

		//Bus_WriteByte(0x96);
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

		Bus_WriteByte(0xCC);
		Bus_WriteByte(0x48);

		printf("%i\n", Data);
		printf("%i\n", Data1);
		printf("%i\n", Data2);
		printf("%i\n", Data3);
		printf("%i\n", Data4);
		printf("%i\n", Data5);
		printf("%i\n", Data6);
		printf("%i\n", Data7);
		printf("%i\n", Data8);

		wait(5000000);
	}
	return 0;
}


