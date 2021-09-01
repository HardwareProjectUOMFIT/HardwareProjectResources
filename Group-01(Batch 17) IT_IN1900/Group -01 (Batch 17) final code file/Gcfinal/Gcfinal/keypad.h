
#include <avr/io.h>

#define KEYPAD_DDR DDRB
#define KEYPAD_PORT PORTB
#define KEYPAD_PIN PINB

extern char scan_for_key();