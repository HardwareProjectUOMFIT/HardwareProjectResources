

#include <avr/delay.h>
#include "keypad.h"

char key_map[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

char scan_for_key(){
	KEYPAD_PORT = 0x0F; //columns 0, rows 1
	
	int r, c;
	
	for (c = 0; c < 4; c++)
	{
		//one column output and 0 state, other columns input and tristate, shift by c times
		//rows output and pull up resistors activated, so at 1 state
		KEYPAD_DDR = 0x10 << c;
		
		for (r = 0; r < 4; r++)
		{
			_delay_ms(.000001); //delay is essential to work properly
			if (!(KEYPAD_PIN & (0x01 << r))) //check column output is in row input
			{
				return key_map[r][c];
			}
		}
	}
	return 'N';
}