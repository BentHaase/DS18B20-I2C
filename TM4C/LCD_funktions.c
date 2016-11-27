/*
 * LCD.c
 *
 *  Created on: 19.10.2016
 *      Author: Sebastian
 *
 *
 */
#include "LCD.h"
#include "BUS.h"
#include "SYSTEM.h"
#include <stdint.h>
#include <stdio.h>
#include "inc/tm4c1294ncpdt.h"
// Grafiken
#include "Numbers.h"
#include "Temp_AusgabeV10_240x160.h"
#include "Temp_Start240x160.h"



void Init() {

	int i = 0;
	//
	// Enable Clock for GPIO-PORTs N, L, M
	// b'0000 0000 0000 0000 0001 1100 0000 0000' => 0x0001C00
	SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R | 0x00001C00;
	//
	// Wait until GPIO-Clock is set.
	//
	i++;

	//
	// Enable the GPIO
	//
	GPIO_PORTL_DIR_R = 0x0000000F;
	GPIO_PORTM_DIR_R = 0x0000000F;
	GPIO_PORTN_DIR_R = 0x0000001F;

	GPIO_PORTL_DATA_R = 0x00000000;
	GPIO_PORTM_DATA_R = 0x00000000;
	GPIO_PORTN_DATA_R = 0x00000000;

	GPIO_PORTL_DEN_R = 0x0000000F;
	GPIO_PORTM_DEN_R = 0x0000000F;
	GPIO_PORTN_DEN_R = 0x0000001F;

}

void InitDisplay()  {

	GPIO_PORTN_DATA_R = 0x00000010;

	wait(10);

	LCD_CmdWrite(EXTIN);  //ext=0 (Befehlssatz)
	LCD_CmdWrite(SLPOUT); // Sleep out
	LCD_CmdWrite(OSCON);  //Oszillator ein

	LCD_CmdWrite(PWRCTRL);
	LCD_DataWrite(0x08);
	wait(2000);
	LCD_CmdWrite(PWRCTRL);
	LCD_DataWrite(0x0B);

	LCD_CmdWrite(VOLCTRL);
	LCD_DataWrite(0x12);
	LCD_DataWrite(0x04);


	LCD_CmdWrite(DISCTRL); // Display Controll
	LCD_DataWrite(0x04);
	LCD_DataWrite(0x27);
	LCD_DataWrite(0x00);

	LCD_CmdWrite(DISINV); // Normal Display

	LCD_CmdWrite(COMSCN); // Scan direction
	LCD_DataWrite(0x01);

	LCD_CmdWrite(DATSDR); //
	// adress and adress scan direction
	LCD_DataWrite(0x01);
	LCD_DataWrite(0x00);
	LCD_DataWrite(0x02);

	LCD_CmdWrite(LASET);
	LCD_DataWrite(0x00);
	LCD_DataWrite(0x9F);

	LCD_CmdWrite(CASET);
	LCD_DataWrite(0x00);
	LCD_DataWrite(0x4F);

	LCD_CmdWrite(EXTOUT); // ext = 1 (Befehlssatz)

	LCD_CmdWrite(ANASET);
	LCD_DataWrite(0x05); // standart oszillator
	LCD_DataWrite(0x00);
	LCD_DataWrite(0x02); // bias

	LCD_CmdWrite(SWINT); // Software Initialisierung
	LCD_ReadEEPROM( );		 // Read EEPROM Flow
	LCD_CmdWrite(EXTIN);
	LCD_CmdWrite(DISON);


}


void LCD_ReadEEPROM( void ) {		// Kontrolliert mit MIDAS-Code

	LCD_CmdWrite( 0x30 );
	LCD_CmdWrite( 0x07 );
	LCD_DataWrite( 0x19 );
	LCD_CmdWrite( 0x31 );
	LCD_CmdWrite( 0xCD );
	LCD_DataWrite( 0x00 );
	wait(1000);
	LCD_CmdWrite( 0xFD );
	wait(1000);
	LCD_CmdWrite( 0xCC );
	LCD_CmdWrite( 0x30 );
}

void LCD_CmdWrite( int Output ) {		// Kontrolliert mit MIDAS-Code

	GPIO_PORTL_DIR_R = 0x0000000F;
	GPIO_PORTM_DIR_R = 0x0000000F;
	GPIO_PORTN_DIR_R = 0x0000000F;

	GPIO_PORTN_DATA_R = 0x00000008;

	GPIO_PORTL_DATA_R = (0x0000000F & Output);
	GPIO_PORTM_DATA_R = (0x000000F0 & Output) >> 4;

	wait(LCD_WAIT);

	GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R | 0x00000005;

	wait(LCD_WAIT);
}


void LCD_DataWrite( int Output ) {		// Kontrolliert mit MIDAS-Code

	GPIO_PORTL_DIR_R = 0x0000000F;
	GPIO_PORTM_DIR_R = 0x0000000F;
	GPIO_PORTN_DIR_R = 0x0000000F;

	GPIO_PORTN_DATA_R = 0x0000000A;

	GPIO_PORTL_DATA_R = (0x0000000F & Output);
	GPIO_PORTM_DATA_R = (0x000000F0 & Output) >> 4;

	wait(LCD_WAIT);

	GPIO_PORTN_DATA_R =  (GPIO_PORTN_DATA_R | 0x00000005);

	wait(LCD_WAIT);



}
int LCD_DataRead( void ) {

	int Input;

	GPIO_PORTL_DIR_R = 0x00000000;
	GPIO_PORTM_DIR_R = 0x00000000;

	GPIO_PORTN_DATA_R = 0x00000006;

	GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R & ~0x00000008);

	wait(LCD_WAIT);

	GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R | 0x00000008);


	wait(LCD_WAIT);

	Input = GPIO_PORTM_DATA_R << 4;
	Input = Input | GPIO_PORTL_DATA_R;

	return Input;

}


// Beispiel

void display_full(int dat) {
	int Com,Seg;
	LCD_CmdWrite(EXTIN); // Ext = 0
	LCD_CmdWrite(CASET); // Column address set
	LCD_DataWrite(0x00);//00//19// From column0 to column240
	LCD_DataWrite(0x4F);//3B//54
	LCD_CmdWrite(LASET); // Page address set
	LCD_DataWrite(0x00);// From line0 to line159
	LCD_DataWrite(0x9F);
	LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

	for(Com = 0; Com< 160;Com++) {
		for(Seg = 0;Seg<240;Seg++) {
			LCD_DataWrite(dat); // Display Data Write
		}
	}
}


void display_gray( void ) {

	int Com,Seg;

	LCD_CmdWrite(EXTIN); // Ext = 0
	LCD_CmdWrite(CASET); // Column address set
	LCD_DataWrite(0x00);//00//19// From column0 to column240
	LCD_DataWrite(0x4F);//3B//54
	LCD_CmdWrite(LASET); // Page address set
	LCD_DataWrite(0x00);// From line0 to line159
	LCD_DataWrite(0x9F);
	LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

	for(Com = 0; Com< 160;Com++) {
		for(Seg = 0;Seg<240;Seg++) {
			LCD_DataWrite(Seg); // Display Data Write
		}
	}
}



void display_pic(void) {
	int i;
	LCD_CmdWrite(EXTIN); // Ext = 0
	LCD_CmdWrite(CASET); // Column address set
	LCD_DataWrite(0x00);//00//19// From column0 to column240
	LCD_DataWrite(0x4F);//3B//54
	LCD_CmdWrite(LASET); // Page address set
	LCD_DataWrite(0x00);// From line0 to line159
	LCD_DataWrite(0x9F);
	LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

	for(i = 0; i< array_length;i++) {
		LCD_DataWrite(standart[i]); // Display Data Write
	}

}


void display_standart(void) {
	int i;
	LCD_CmdWrite(EXTIN); // Ext = 0
	LCD_CmdWrite(CASET); // Column address set
	LCD_DataWrite(0x00);//00//19// From column0 to column240
	LCD_DataWrite(0x4F);//3B//54
	LCD_CmdWrite(LASET); // Page address set
	LCD_DataWrite(0x00);// From line0 to line159
	LCD_DataWrite(0x9F);
	LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

	for(i = 0; i< array_length;i++) {
		LCD_DataWrite(ausgabe[i]); // Display Data Write
	}

}

boolean display_plot_temperature( double temperatur, int Sensor ) {
	int x = 0, y = 0, i, k, j, b, temp = 0, rest;
	int Output [6] = {{0}};

	if ( Sensor > 4) {
		return False;
	}

	temp = (int) (temperatur * 1000);

	rest = temp % 100000;
	Output[0] = (temp-rest) / 100000;
	temp = temp - (100000 * Output[0]);

	rest = temp % 10000;
	Output[1] = (temp-rest) / 10000;
	temp = temp - (10000 * Output[1]);

	rest = temp % 1000;
	Output[2] = (temp-rest) / 1000;
	temp = temp - (1000 * Output[2]);

	rest = temp % 100;
	Output[3] = (temp-rest) / 100;
	temp = temp - (100 * Output[3]);

	rest = temp % 10;
	Output[4] = (temp-rest) / 10;
	temp = temp - (10 * Output[4]);

	rest = temp % 1;
	Output[5] = (temp-rest) / 1;
	temp = temp - (1 * Output[5]);





	x = 123/3;

	for ( i = 0; i <= 5; i++ ) {

		y = 12 + Sensor * 28;

		if( i == 0 && Output[0] == 0 || i == 1 && Output[1] == 0 ) {
			x = x + 5;
			i++;
		}

		switch( Output[i] ) {
		case 0:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); // Ext = 0
				LCD_CmdWrite(CASET); // Column address set
				LCD_DataWrite(x);//00//19// From column0 to column240
				LCD_DataWrite(x+5);//3B//54
				LCD_CmdWrite(LASET); // Page address set
				LCD_DataWrite(y);// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num0[b]); // Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num0[b]); // Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num0[b]); // Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 1:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); // Ext = 0
				LCD_CmdWrite(CASET); // Column address set
				LCD_DataWrite(x);//00//19// From column0 to column240
				LCD_DataWrite(x+5);//3B//54
				LCD_CmdWrite(LASET); // Page address set
				LCD_DataWrite(y);// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num1[b]); // Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num1[b]); // Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num1[b]); // Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 2:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); // Ext = 0
				LCD_CmdWrite(CASET); // Column address set
				LCD_DataWrite(x);//00//19// From column0 to column240
				LCD_DataWrite(x+5);//3B//54
				LCD_CmdWrite(LASET); // Page address set
				LCD_DataWrite(y);// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num2[b]); // Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num2[b]); // Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num2[b]); // Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 3:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); // Ext = 0
				LCD_CmdWrite(CASET); // Column address set
				LCD_DataWrite(x);//00//19// From column0 to column240
				LCD_DataWrite(x+5);//3B//54
				LCD_CmdWrite(LASET); // Page address set
				LCD_DataWrite(y);// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num3[b]); // Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num3[b]); // Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num3[b]); // Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 4:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); // Ext = 0
				LCD_CmdWrite(CASET); // Column address set
				LCD_DataWrite(x);//00//19// From column0 to column240
				LCD_DataWrite(x+5);//3B//54
				LCD_CmdWrite(LASET); // Page address set
				LCD_DataWrite(y);// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num4[b]); // Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num4[b]); // Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num4[b]); // Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 5:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); 					// Ext = 0
				LCD_CmdWrite(CASET); 					// Column address set
				LCD_DataWrite(x);						//00//19// From column0 to column240
				LCD_DataWrite(x+5);						//3B//54
				LCD_CmdWrite(LASET); 					// Page address set
				LCD_DataWrite(y);						// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  					// Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num5[b]); 			// Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num5[b]);				// Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num5[b]); 			// Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 6:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); 					// Ext = 0
				LCD_CmdWrite(CASET); 					// Column address set
				LCD_DataWrite(x);						//00//19// From column0 to column240
				LCD_DataWrite(x+5);						//3B//54
				LCD_CmdWrite(LASET); 					// Page address set
				LCD_DataWrite(y);						// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  					// Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num6[b]); 			// Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num6[b]); 			// Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num6[b]); 			// Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 7:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); 					// Ext = 0
				LCD_CmdWrite(CASET); 					// Column address set
				LCD_DataWrite(x);						//00//19// From column0 to column240
				LCD_DataWrite(x+5);						//3B//54
				LCD_CmdWrite(LASET); 					// Page address set
				LCD_DataWrite(y);						// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  					// Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num7[b]); 			// Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num7[b]); 			// Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num7[b]); 			// Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 8:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); 					// Ext = 0
				LCD_CmdWrite(CASET); 					// Column address set
				LCD_DataWrite(x);						//00//19// From column0 to column240
				LCD_DataWrite(x+5);						//3B//54
				LCD_CmdWrite(LASET); 					// Page address set
				LCD_DataWrite(y);						// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  					// Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num8[b]); // Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num8[b]); // Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num8[b]); // Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		case 9:
			for( k = 0; k < 22; k++ ) {

				LCD_CmdWrite(EXTIN); // Ext = 0
				LCD_CmdWrite(CASET); // Column address set
				LCD_DataWrite(x);//00//19// From column0 to column240
				LCD_DataWrite(x+5);//3B//54
				LCD_CmdWrite(LASET); // Page address set
				LCD_DataWrite(y);// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

				for ( j = 0; j < 5; j++ ) {

					b = j*3+k*15;
					LCD_DataWrite(num9[b]); // Display Data Write num0[j*3]

					b = j*3+k*15+1;
					LCD_DataWrite(num9[b]); // Display Data Write num0[j*3+1]

					b = j*3+k*15+2;
					LCD_DataWrite(num9[b]); // Display Data Write num0[j*3+2]
				}

				y++;
			}
			x = x + 5;
			break;

		default:
		}

		if (i == 2) {

			y = 12 + Sensor * 28;

			for ( j = 0; j < 22; j++ ) {

				LCD_CmdWrite(EXTIN); // Ext = 0
				LCD_CmdWrite(CASET); // Column address set
				LCD_DataWrite(x);//00//19// From column0 to column240
				LCD_DataWrite(x);//3B//54
				LCD_CmdWrite(LASET); // Page address set
				LCD_DataWrite(y);// From line0 to line159
				LCD_DataWrite(y);
				LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

				if ( j >19 ) {
					LCD_DataWrite(0xFF); // Display Data Write num0[j*3]
					LCD_DataWrite(0xFF); // Display Data Write num0[j*3+1]
					LCD_DataWrite(0x00); // Display Data Write num0[j*3+2]

				} else {
					LCD_DataWrite(0x00); // Display Data Write num0[j*3]
					LCD_DataWrite(0x00); // Display Data Write num0[j*3+1]
					LCD_DataWrite(0x00); // Display Data Write num0[j*3+2]
				}
				y++;
			}
			x = x+1;
		}
	}

	y = 12 + Sensor * 28;

	for ( j = 0; j < 22; j++ ) {

		LCD_CmdWrite(EXTIN); // Ext = 0
		LCD_CmdWrite(CASET); // Column address set
		LCD_DataWrite(x);//00//19// From column0 to column240
		LCD_DataWrite(x);//3B//54
		LCD_CmdWrite(LASET); // Page address set
		LCD_DataWrite(y);// From line0 to line159
		LCD_DataWrite(y);
		LCD_CmdWrite(0x5C);  // Entry Memory Write Mode

		if ( j == 0 || j == 2 ) {
			LCD_DataWrite(0xFF); // Display Data Write num0[j*3]
			LCD_DataWrite(0xFF); // Display Data Write num0[j*3+1]
			LCD_DataWrite(0xFF); // Display Data Write num0[j*3+2]
		} else if ( j == 1) {
			LCD_DataWrite(0xFF); // Display Data Write num0[j*3]
			LCD_DataWrite(0x00); // Display Data Write num0[j*3+1]
			LCD_DataWrite(0xFF); // Display Data Write num0[j*3+2]
		} else {
			LCD_DataWrite(0x00); // Display Data Write num0[j*3]
			LCD_DataWrite(0x00); // Display Data Write num0[j*3+1]
			LCD_DataWrite(0x00); // Display Data Write num0[j*3+2]
		}
		y++;
	}


	return True;
}































