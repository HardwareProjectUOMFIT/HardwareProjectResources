/*
 * final.c
 *
 * Created: 03/06/2018 20:45:02
 * Author : gayav
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

# define F_CPU 1000000UL

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "lcd.h"
#include <math.h>
#include "keypad.h"

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
'6','3','0','9','5','2',
'8','2','4','9','3','6','5'
};

char* vision_score[7]={"6/60","6/36","6/24","6/18","6/12","6/9","6/6"};

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

void selecting(uint8_t eye){
	Lcd4_Clear();
	if(eye==1){
		Lcd4_Write_String("Close left eye");
	}else{
		Lcd4_Write_String("Close right eye");
	}
	_delay_ms(3000);
	
	
	Lcd4_Clear();
	Lcd4_Write_String("Vision test 1");
	_delay_ms(3000);
	Lcd4_Clear();
	uint8_t wrong_count = 0;
	uint8_t level = 0;
	char key;
	/*
	//first row
	Lcd4_Write_String("Select option");
	shift_write(0x01,0);
	key = getkey();
	if(key!='5'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/60");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	//second row
	wrong_count =0;
	
	Lcd4_Write_String("Select option");
	shift_write(0x02,0);
	key = getkey();
	if(key!='9'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/60");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x04,0);
	key = getkey();
	if(key!='4'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/60");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	//row 3
	Lcd4_Write_String("Select option");
	shift_write(0x08,0);
	key = getkey();
	if(key!='2'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/36");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x10,0);
	key = getkey();
	if(key!='6'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/36");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x10,0);
	key = getkey();
	if(key!='0'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/36");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	//row4
	Lcd4_Write_String("Select option");
	shift_write(0x20,0);
	key = getkey();
	if(key!='3'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/24");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x40,0);
	key = getkey();
	if(key!='8'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/24");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x80,0);
	key = getkey();
	if(key!='9'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/24");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x01,1);
	key = getkey();
	if(key!='5'){
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/24");
		_delay_ms(2000);
		return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	//row5
	Lcd4_Write_String("Select option");
	shift_write(0x02,1);
	key = getkey();
	if(key!='0'){
		wrong_count++;
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		_delay_ms(1500);
		//return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x04,1);
	key = getkey();
	if(key!='6'){
		wrong_count++;
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		Lcd4_Set_Cursor(2,0);
		_delay_ms(1500);
		//return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	Lcd4_Write_String("Select option");
	shift_write(0x08,1);
	key = getkey();
	if(key!='2'){
		wrong_count++;
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		_delay_ms(1500);
		//return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	if(wrong_count==3){
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/12");
		_delay_ms(2000);
		return;
	}
	
	Lcd4_Write_String("Select option");
	shift_write(0x10,1);
	key = getkey();
	if(key!='4'){
		wrong_count++;
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		_delay_ms(1500);
		//return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	if(wrong_count==3){
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/12");
		_delay_ms(2000);
		return;
	}
	
	Lcd4_Write_String("Select option");
	shift_write(0x20,1);
	key = getkey();
	if(key!='8'){
		wrong_count++;
		Lcd4_Clear();
		Lcd4_Write_String("Wrong");
		_delay_ms(1500);
		//return;
	}
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Correct");
	_delay_ms(1500);
	Lcd4_Clear();
	
	if(wrong_count>=3){
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/12");
		_delay_ms(2000);
		return;
	}else if(wrong_count==2){
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("Score 6/12");
		_delay_ms(2000);
	}else if(wrong_count==1){
		
	}else{
		
	}
	
	*/
	shift_write(0b00000001,4);
	_delay_ms(1000);
	shift_write(0b00000010,4);
	_delay_ms(1000);
	shift_write(0b00000100,4);
	_delay_ms(1000);
	shift_write(0b00001000,4);
	_delay_ms(1000);
	shift_write(0b00010000,4);
	_delay_ms(1000);
	shift_write(0b00100000,4);
	_delay_ms(1000);
	shift_write(0b01000000,4);
	_delay_ms(1000);
	shift_write(0b10000000,4);
	_delay_ms(1000);
	
	
	char ans;
	int i,j,pin_count=0,pin=0;
	uint8_t val=0;
	for(i=0;i<7;i++){
		wrong_count=0;
		for(j=0;j<=i;j++){
			Lcd4_Write_String("Select option");
			Lcd4_Set_Cursor(2,0);
			if(pin_count==9){
				pin_count=1;
				pin++;
			}
			val = 0;
			shift_write((1<<pin_count),pin);
			pin_count++;
			ans=getkey();
			if(ans==vision1[level]){
				Lcd4_Write_String("Correct..");
			}else{
				Lcd4_Write_String("Wrong..");
				wrong_count++;
			}
			level++;
			_delay_ms(2000);
			Lcd4_Clear();
			if(wrong_count==3){
				Lcd4_Write_String("Your vision score");
				Lcd4_Set_Cursor(2,0);
				Lcd4_Write_String(vision_score[i-1]);
				break;
			}else if(wrong_count==1 && i==0){
				Lcd4_Write_String("Your vision score");
				Lcd4_Set_Cursor(2,0);
				Lcd4_Write_String(vision_score[0]);
				break;
			}
		}
	}
	if(wrong_count==0){
		Lcd4_Clear();
		Lcd4_Write_String("Your vision score");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String(vision_score[i]);
	}else if(wrong_count==1){
		Lcd4_Clear();
		Lcd4_Write_String("Your vision score");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String(vision_score[i]);
		Lcd4_Write_String("-1");
	}else if(wrong_count==2){
		Lcd4_Clear();
		Lcd4_Write_String("Your vision score");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String(vision_score[i]);
		Lcd4_Write_String("-2");
	}
	_delay_ms(4000);
	
	Lcd4_Clear();
	Lcd4_Write_String("Vision test 2");
	_delay_ms(3000);
	Lcd4_Clear();
	
	wrong_count = 0;
	level = 0;
	for(i=0;i<7;i++){
		wrong_count=0;
		for(j=0;j<7;j++){
			Lcd4_Write_String("Select option");
			Lcd4_Set_Cursor(2,1);
			if(pin_count==7){
				pin_count=0;
				pin++;
			}
			shift_write(pow(2,pin_count),pin);
			pin_count++;
			ans=getkey();
			if(ans==vision2[level]){
				Lcd4_Write_String("Correct..");
				}else{
				Lcd4_Write_String("Wrong..");
				wrong_count++;
			}
			level++;
			_delay_ms(2000);
			Lcd4_Clear();
			if(wrong_count==2){
				Lcd4_Write_String("Your vision score");
				Lcd4_Set_Cursor(2,0);
				if(i==0){
					Lcd4_Write_String(vision_score[0]);
					}else{
					Lcd4_Write_String(vision_score[i-1]);
				}
				break;
			}
		}
	}
	if(wrong_count==0){
		Lcd4_Clear();
		Lcd4_Write_String("Your vision score");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String(vision_score[i]);
		}else if(wrong_count==1){
		Lcd4_Clear();
		Lcd4_Write_String("Your vision score");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String(vision_score[i]);
		Lcd4_Write_String("-1");
		}
	
}

void showresults(){
	
	Lcd4_Clear();
	Lcd4_Write_String("your Result :- ");
	convertstr(count);
	Lcd4_Write_String(str);
	_delay_ms(3000);
	Lcd4_Clear();
	count=0;
	
	
}



int main(void){
	//MCUCSR=(1<<JTD);
	//MCUCSR=(1<<JTD);
	DDRD = 0xFB;
	DDRC = 0xFF;
	DDRB = 0x00;	// for keybord
	DDRA = 0xFF;	// for bulb
	
	DDRA &= ~(1<<PA2 | 1<<PA3);
	
	PORTD = 0x00;
	PORTA = 0x00;
	PORTC = 0x00;
	
	_delay_ms(50);
	
	GICR|=(1<<INT0);
	MCUCR|=(1<<ISC00);

	TCCR1A = 0;
	int16_t COUNTA = 0;
	
	Lcd4_Init();
	shift_init();
	
	_delay_ms(2000);
	
	sei();
	
	Lcd4_Clear();
	Lcd4_Write_String("***KK***");
	
	board_set_front();
	board_set_back();
	
  while(1){  
		
		//_delay_ms(4000);
	  	//Lcd4_Clear();
		  
		  
		  
		 
		 
		 
		  
		  
		 //selecting(1);

		//board_rotate_front();
		//_delay_ms(2000);
		//board_set_front();
		//selecting();
		//board_rotate_back();
		//_delay_ms(2000);
		//board_set_back();
		//selecting();
	}
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
	//while((PINA & (1<<PA2)));
	 _delay_ms(1000);
	PORTD &= ~(1<<PD2);
}

void board_set_back(){
	PORTD |= (1<<PD3);
	PORTD &= ~(1<<PD2);
	//while((PINA & (1<<PA3)));
	_delay_ms(1000);
	PORTD &= ~(1<<PD3);
}

void board_rotate_front(){
	for(int i=0;i<20;i++){
		PORTC |= (1<<PC0);
		_delay_us(1700);
		PORTC &= ~(1<<PC0);
		_delay_us(18300);
	}
}

void board_rotate_back(){
	for(int i=0;i<20;i++){
		PORTC |= (1<<PC0);
		_delay_us(900);
		PORTC &= ~(1<<PC0);
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
