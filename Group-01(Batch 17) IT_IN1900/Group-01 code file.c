/*
 * final.c
 *
 * Created: 03/06/2018 20:45:02
 * Author : gayav
 */ 

/*
 * vision control b 17.c
 *
 * Created: 2018-05-27 12:19:52 AM
 * Author : Ishara
 */ 

/*
 * final_code
 *
 * Created: 2018-09-02 12:19:52 AM
 * Author : Avarjana
 */ 

/*

keypad protB

ir -> front pd0
ir -> back pd1

moter out = pd2,pd3
lcd  pd4-pd7  pc6 pc7

vision bord bulb 
	data-> a0	latch -> a1
	shift 1	->	clk a2
	shift 2	->	clk a3
	shift 3	->	clk a4
	shift 4	->	clk a5
	shift 5 ->	clk a6
	shift 6 ->	clk a7
	shift 7 ->	clk c0
	shift 8 ->	clk c1
	
servo c2
*/

# define F_CPU 8000000UL

#define D4 eS_PORTD3
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include <string.h>
#include <math.h>
#include "keypad.h"
#include "shift.h"

#define LIMIT_FRONT PA3
#define LIMIT_BACK PA2
#define GEAR_A PA6
#define GEAR_B PA7

void forward();
void backward();
void initPorts();
int limitSwitch(int);
void gear(int a, int b);

void servo();

char str[4], op;
int count =0;
static volatile int pulse = 0;
static volatile int i = 0;

void shift_init();
void shift_latch();
void shift_high_bit();
void shift_low_bit();
void shift_pulse(uint8_t);
void shift_write(uint8_t,uint8_t);
void board_set_front();
void board_set_back();
void board_rotate_front();
void board_rotate_back();

void convertstr(int a){
	
	itoa(a,str,10);
}

char getkey(){
	char a='N';
	while(a=='N'){
		a=scan_for_key();
	}	
	return a;
}



char vision1[29] = {
'5',
'9','4',
'2','6','0',
'3','8','9','5',
'0','6','2','4','8',
'6','3','0','9','5','2'
};


void print(char *arr){
	Lcd4_Clear();
	Lcd4_Write_String(arr);
}
void shift_reset(){
	HC595Write(0x00);
	HC595Write(0x00);
	HC595Write(0x00);
	HC595Write(0x00);
}

void shift_new(uint32_t x){
	HC595Write((x >> 24) & 0xFF);
	HC595Write((x >> 16) & 0xFF);
	HC595Write((x >> 8) & 0xFF);
	HC595Write((x) & 0xFF);
}

char keyRead(){
	char x = 'l';
	while(x=='l' || x=='N'){
		x = scan_for_key();
	}
	return x;
}

char* vision_score[8]={"0 - Fail","6/60","6/36","6/24","6/18","6/12","6/9","6/6"};


void eyeTest(){
	shift_reset();
	char arr[10];
	char v;
	
	int lim = 1;
	int count = 0;
	int wrong = 0;
	
	uint32_t x = 2;
	for(int i=0;i<21;i++){
		
		count++;
		if(i==16){
			x*=2;
			i++;
		}
		if(i==7){
			i++;
		}
		shift_new(x);
		print("Enter value");
		v = keyRead();
		
		if(v == vision1[i]){
			print("Correct");
		}else{
			print("Incorrect");
			wrong++;
		}
		
		if(wrong*2 >= lim){
			print("Test Done");
			_delay_ms(4000);
			sprintf(arr,"Score=%s",vision_score[lim-1]);
			print(arr);
			_delay_ms(4000);
			break;
		}
		
		if(count+1 > lim){
			lim += 1;
			count = 0;
			wrong = 0;
		}
		//sprintf(arr, "char=%c", vision1[i]);
		//print(arr);
		x *= 2;
		_delay_ms(1000);
	}
}





//char vision1_out

char vision2[35] = {
'6','9','5','8','9',
'5','9','6','9','8',
'6','8','9','6','5',
'9','8','5','6','9',
'8','9','6','5','8',
'5','6','9','8','6',
'6','5','8','9','8'	
};



void showresults(){
	
	Lcd4_Clear();
	Lcd4_Write_String("your Result :- ");
	convertstr(count);
	Lcd4_Write_String(str);
	_delay_ms(3000);
	Lcd4_Clear();
	count=0;
	
	
}

void colorTest(){
	print("Color test");
	_delay_ms(2000);
	shift_reset();
	uint32_t x = pow(2,25);
	char c;
	char arr[10];
	for(int i=0;i<6;i++){
		if(i==5){
			x *= 2;
		}
		
		// see - 1 not see - 0
		
		shift_new(x);
		print("Enter value");
		c = keyRead();
		
		if(i==0 || i==4){
			if(c == '0'){
				print("Correct");
				_delay_ms(2000);
			}else{
				print("Incorrect");
				_delay_ms(2000);
			}
		}else{
			if(c == '1'){
				print("Correct");
				_delay_ms(2000);
			}else{
				print("Incorrect");
				_delay_ms(2000);
			}
		}
		
		//print(arr);
		x *= 2;
		_delay_ms(2000);
	}
}


int main(){
	DDRD = 0xFF;
	DDRC = 0xFF;
	DDRB = 0xf0;	// for keybord
	DDRA = 0xFF;	// for bulb
	DDRA &= ~(0<<LIMIT_BACK);
	DDRA &= ~(0<<LIMIT_FRONT);
	PORTD = 0x00;
	PORTA = 0x00;
	PORTA |= (1<<LIMIT_FRONT) | (1<<LIMIT_BACK);
	PORTC = 0x00;
	
	GICR|=(1<<INT0);
	MCUCR|=(1<<ISC00);

	TCCR1A = 0;
	
	Lcd4_Init();
	_delay_ms(1000);
	
	shift_reset();
	
	Lcd4_Clear();
	Lcd4_Write_String("hello");
	_delay_ms(2000);
	
	forward();
	
	char buff[10];
	uint32_t m = 1;
	while(1){
		eyeTest();
		backward();
		_delay_ms(2000);
		colorTest();
		_delay_ms(2000);
		forward();
	}
}



void forward(){
	PORTA |= (1<<GEAR_A);
	PORTA &= ~(1<<GEAR_B);
	while(PINA & 0x08){
		_delay_ms(100);
	}
	
	PORTA &= ~((1<<GEAR_B) | (1<<GEAR_A));
}

void backward(){
	PORTA &= ~(1<<GEAR_A);
	PORTA |= (1<<GEAR_B);
	while(PINA & 0x04){
		_delay_ms(100);
	}
	
	PORTA &= ~((1<<GEAR_B) | (1<<GEAR_A));
}

void shift_init(){
	DDRA = 0xFF;
	DDRC |= 0x03;
	PORTA = 0x00;
}

void shift_latch(){
	PORTA |= 0x02;
	PORTA &= ~0x02;
}

void shift_high_bit(){
	PORTA |= 0x01;
}

void shift_low_bit(){
	PORTA &= ~0x01;
}

void shift_pulse(uint8_t reg_num){
	switch(reg_num){
		case 0:
			PORTA |= 0x04;
			PORTA &= ~0x04;
			break;
		case 1:
			PORTA |= 0x08;
			PORTA &= ~0x08;
			break;
		case 2:
			PORTA |= 0x10;
			PORTA &= ~0x10;
			break;
		case 3:
			PORTA |= 0x20;
			PORTA &= ~0x20;
			break;
		case 4:
			PORTA |= 0x40;
			PORTA &= ~0x40;
			break;
		case 5:
			PORTA |= 0x80;
			PORTA &= ~0x80;
			break;
		case 6:
			PORTC |= 0x01;
			PORTC &= ~0x01;
			break;
		case 7:
			PORTC |= 0x02;
			PORTC &= ~0x02;
			break;
	}
}

void shift_write(uint8_t data,uint8_t reg_num){
	int i;
	PORTA |= 0x02;
	
	for(i=0;i<8;i++)
	{
		if(data & 0b10000000)
		{
			shift_high_bit();
		}else{
			shift_low_bit();
		}
		shift_pulse(reg_num);
		data=data<<1;
	}
	//shift_latch();
	PORTA &= ~0x02;
}

void board_set_front(){
	PORTD |= (1<<PD2);
	PORTD &= ~(1<<PD3);
	//while(!(PIND & (1<<PD0)));
	_delay_ms(5000);
	PORTD &= ~(1<<PD2);
}

void board_set_back(){
	PORTD |= (1<<PD3);
	PORTD &= ~(1<<PD2);
	//while(!(PIND & (1<<PD1)));
	_delay_ms(5000);
	PORTD &= ~(1<<PD3);
}

void board_rotate_front(){
	for(int i=0;i<12;i++){
		PORTC |= (1<<PC2);
		_delay_us(1700);
		PORTC &= ~(1<<PC2);
		_delay_us(18300);
	}
}

void board_rotate_back(){
	for(int i=0;i<12;i++){
		PORTC |= (1<<PC2);
		_delay_us(900);
		PORTC &= ~(1<<PC2);
		_delay_us(19100);
	}
}

ISR (INT0_vect){
	if (i==1)
	{
		TCCR1B=0;
		pulse=TCNT1;
		TCNT1=0;
		i=0;
	}
	if (i==0)
	{
		TCCR1B|=(1<<CS10);
		i=1;
	}
}
