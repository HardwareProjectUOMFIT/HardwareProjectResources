/*
 * shift.h
 *
 * Created: 2/25/2020 9:23:46 PM
 *  Author: Avarjana
 */ 


#ifndef SHIFT_H_
#define SHIFT_H_





#endif /* SHIFT_H_ */

#include <avr/io.h>
#include <util/delay.h>

#define HC595_PORT   PORTA
#define HC595_DDR    DDRA

#define HC595_DS_POS PA0

#define HC595_SH_CP_POS PA4
#define HC595_ST_CP_POS PA1

void HC595Init(){
	HC595_DDR|=((1<<HC595_SH_CP_POS)|(1<<HC595_ST_CP_POS)|(1<<HC595_DS_POS));
}



//Low level macros to change data (DS)lines
#define HC595DataHigh() (HC595_PORT|=(1<<HC595_DS_POS))

#define HC595DataLow() (HC595_PORT&=(~(1<<HC595_DS_POS)))

//Sends a clock pulse on SH_CP line
void HC595Pulse()
{
	//Pulse the Shift Clock

	HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH

	HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW

}

//Sends a clock pulse on ST_CP line
void HC595Latch()
{
	//Pulse the Store Clock

	HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
	_delay_loop_1(1);

	HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
	_delay_loop_1(1);
}



void HC595Write(uint8_t data)
{
	//Send each 8 bits serially

	//Order is MSB first
	for(uint8_t i=0;i<8;i++)
	{
		//Output the data on DS line according to the
		//Value of MSB
		if(data & 0b10000000)
		{
			//MSB is 1 so output high

			HC595DataHigh();
		}
		else
		{
			//MSB is 0 so output high
			HC595DataLow();
		}

		//_delay_ms(100);
		HC595Pulse();  //Pulse the Clock line
		data=data<<1;  //Now bring next bit at MSB position

	}
	
	//_delay_ms(100);

	//Now all 8 bits have been transferred to shift register
	//Move them to output latch at one
	HC595Latch();
}


