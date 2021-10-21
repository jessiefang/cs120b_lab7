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

enum States {Start, Init, Increment, Decrement, Reset, On_Increment, On_Decrement} state;
unsigned char cnt = 0x07;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned char i = 0;
unsigned char timeTrigger = 0;
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

void Tick() {
unsigned char tmpA = ~PINA & 0x03;
 switch(state){
		case Start:
			state = Init;
                        cnt = 0;
			break;
		case Init:
			if (tmpA == 0x03){
				state = Reset;
			}
			else if(tmpA == 0x01){
				state = On_Increment;
			}
			else if(tmpA == 0x02){
				state = On_Decrement;
			}
			break;
		case Increment:
			if(tmpA == 0x01) {
				timeTrigger = 1;
				//Check to see if it still 0x01
				//We want to now start keeping track
				//10 cycles
				if(i == 10) {
				state = On_Increment;
				}
				else {
					state = Increment;
				}
			}
			else if (tmpA == 0x00){
				state = Init;
			}else if(PINA == 0x02){
				state = On_Decrement;
			}
			else if (tmpA == 0x03){
				PORTC = 0x00;
				state = Reset;
			}
                        break;
		case On_Increment:
			state = Increment;
			i = 0;
			timeTrigger = 0;
			break;
		case Decrement:
                        if(tmpA == 0x02){
				timeTrigger = 1;
                                //Check to see if it still 0x02
                                //We want to now start keeping track
                                //10 cycles
                                if(i == 10) {
                                state = On_Decrement;
                                }
                                else {
                                        state = Decrement;
                                }
                        }else if (tmpA == 0x00){
                                state = Init;
                        }else if (tmpA == 0x01){
				state = On_Increment;
			}
			else if (tmpA == 0x03){
                                state = Reset;
                        }
                        break;
		case On_Decrement:
                        state = Decrement;
	                i =0;
			timeTrigger = 0;
			break;
		case Reset:
			if(PINA == 0x03){
				state = Reset;
			}else if(PINA == 0x00){
				state = Init;
			}
			else{
				state = Init;
			}
			break;
		 default:
			state = Start;
			break;
	}
	switch(state){
		case Start:
			PORTB = 0x07;
			break;
		case Init:
			break;
		case Increment:
			break;
		case On_Increment:
			if(cnt < 0x09){
				cnt++;
				LCD_ClearScreen();
          			LCD_WriteData(cnt + '0');
			}
			break;
		case Decrement:
			break;
		case On_Decrement:
			if(cnt > 0x00){
				cnt--;
				LCD_ClearScreen();
          			LCD_Cursor(1);
				LCD_WriteData(cnt + '0');	
			}
			break;
		case Reset:
			LCD_ClearScreen();
			LCD_WriteData(0 + '0');
			PORTB = 0x00;
			cnt = 0;
			break;
		default:
			PORTB = 0x07;
			break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
    TimerSet(100);
    TimerOn();
    LCD_init();
    LCD_Cursor(1);
    LCD_WriteData(0 + '0');
    
    while (1) {
    	Tick();
	while(!TimerFlag) {};
	TimerFlag = 0;
	if(timeTrigger == 1) {
	   i++;
	}
	PORTB = cnt;

    }
    return 1;
}
