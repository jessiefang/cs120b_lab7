/*	Author: Yunjie Fang
 *  Partner(s) Name: 
 *	Lab Section:021
 *	Assignment: Lab #7  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link:
 */
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start, led1, led2, led3, pause, win, restart1} state;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
 TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
unsigned char flag = 0;
unsigned char score = 5;

void Tick() {
unsigned char press = ~PINA & 0x01;

 switch(state) {
   case Start:
   LCD_ClearScreen();
   LCD_WriteData(score + '0');
   state = led1;   break;

   case led1:
   flag  = 0;
   if(press){
	   if(score > 0){
		score--;
		LCD_ClearScreen();
      		LCD_WriteData(score + '0');
	   }
	   state = pause;
   }
   else{
	state = led2;
   }
   break;

   case led2:
   if(press){
	   if(score < 9){
                score++;
                LCD_ClearScreen();
                LCD_WriteData(score + '0');
           }
           state = pause;
   }
   else if(flag == 0) {
           state = led3;
   }
   else if(flag == 1) {
	   state = led1;
   }
   break;

   case led3:
   flag = 1;
   if(press){
	   if(score > 0){
                score--;
                LCD_ClearScreen();
                LCD_WriteData(score + '0');
           }

           state = pause;
   }
   else{
           state = led2;
   }

   break;

   case pause:
   if(press){
	   state = pause;
   }else if (score == 9){
	   LCD_ClearScreen();
     	   LCD_DisplayString(1, "Winner");
     	   state = win;
   }
   else{
	   state = restart1;
   }
   break;

   case restart1:
   if(press){
	   state = Start;
   }

   break;

   case win:
	   if(press){
     		   score = 5;
     		   LCD_ClearScreen();
     	   	   LCD_WriteData(score + '0');
     	   	   state = restart1;
   	   }
   	   else {
     		  state = win;
           }
   	   break;

   default:
   	state = Start;
	break;
 }

 switch(state) {
   case Start:  break;

   case led1:
   PORTB = 0x01;
   break;

   case led2:
   PORTB = 0x02;  break;

   case led3:
   PORTB = 0x04;  break;

   case pause:
   break;

   case restart1:
   break;
 
   case win:
   break;

 }
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    TimerSet(300);
    TimerOn();
    LCD_init();
    LCD_Cursor(1);

    state = Start;
    score = 5;
    /* Insert your solution below */
    while (1) {
    	Tick();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
