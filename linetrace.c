#include <xc.h>
#pragma config FOSC =HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#define _XTAL_FREQ 10000000

void servocenter(void);
double servoctl(double set);

char PULSE;
main(void) {
	TRISA = 0x1F;
	TRISB = 0x00;
	OPTION_REG = 0b00000111;
	PULSE = 150;
	TMR0 = 61;
	INTCON = 0b10100000;
	PORTB = 0b00001000;
	double pld = 150;
	int flag = 0;
	int blackcount = 0;

	while (1) {
		if (PORTAbits.RA4 == 1) {
			flag = 1;
		}

		if (flag == 1) {

			PORTBbits.RB7 = PORTBbits.RB5 = 0;
			PORTBbits.RB6 = PORTBbits.RB4 = 1;
			
			if (PORTAbits.RA0 == 1 && PORTAbits.RA1 == 1 && PORTAbits.RA2 == 1){
				blackcount++;
				if(blackcount > 80){
					pld = 300 - pld;
					servoctl(pld);
					PORTBbits.RB7 = PORTBbits.RB5 = 1;
					PORTBbits.RB6 = PORTBbits.RB4 = 1;
					__delay_ms(200);
					PORTBbits.RB7 = PORTBbits.RB5 = 1;
					PORTBbits.RB6 = PORTBbits.RB4 = 0;
					__delay_ms(400);
					PORTBbits.RB7 = PORTBbits.RB5 = 0;
					PORTBbits.RB6 = PORTBbits.RB4 = 0;
					__delay_ms(200);
					blackcount = 0;
				}
			}
			else if (PORTAbits.RA0 == 0 && PORTAbits.RA2 == 1 && pld > 100.0) {
				pld = pld - 1.3;
				blackcount = 0;
				servoctl(pld);
			}
			else if (PORTAbits.RA0 == 1 && PORTAbits.RA2 == 0 && pld < 200.0) {
				pld = pld + 1.3;
				blackcount = 0;
				servoctl(pld);
			}
			__delay_ms(2);
			PORTBbits.RB7 = PORTBbits.RB5 = 0;
			PORTBbits.RB6 = PORTBbits.RB4 = 0;
			__delay_ms(5);
		}
	}
	return 0;
}

double servoctl(double set) {
	PULSE = set;
	return 0;
}
void servocenter(void) {
	PULSE = 150;
	return;
}

void interrupt isr(void) {
	if (INTCONbits.T0IF) {
		TMR0 = 61;
		char pulse_temp = PULSE;
		PORTB = PORTB & 0b11111110;
		while (pulse_temp > 0) {
			__delay_ms(0.007);
			--pulse_temp;
		}
		PORTB = PORTB | 0b00000001;
		INTCONbits.T0IF = 0;
	}
}
