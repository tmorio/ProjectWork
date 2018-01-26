#include <xc.h>
#pragma config FOSC =HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#define _XTAL_FREQ 10000000
char PULSE;
main(void) {
	TRISA = 0x1F;
	TRISB = 0x00;
	OPTION_REG = 0b00000111;
	PULSE = 150;		//サーボモーター初期位置定義
	TMR0 = 61;
	INTCON = 0b10100000;
	PORTB = 0b00001000;
	int pld = 150;		//サーボモーター位置変数定義
	int flag = 0;		//起動フラグ定義
	while (1) {			//ループ開始
		if (PORTAbits.RA4 == 1) {	//起動スイッチ待機
			flag = 1;				//起動フラグを設定
		}
		if (flag == 1) {			//起動フラグ確認
			PORTBbits.RB7 = PORTBbits.RB5 = 0;	//前進
			PORTBbits.RB6 = PORTBbits.RB4 = 1;
			if (PORTAbits.RA0 == 0 && PORTAbits.RA2 == 1 && pld > 100) {	//サーボ右回転用及び、回転可能上限の設定
				pld = pld - 10;	//角度設定
				PULSE = pld;	//サーボモーター動作実行
			}else if (PORTAbits.RA0 == 1 && PORTAbits.RA2 == 0 && pld < 200) {	//サーボ左回転用及び、回転可能上限の設定
				pld = pld + 10;	//角度設定
				PULSE = pld;	//サーボモーター動作実行
			}
			__delay_ms(4);		//4msウェイト
			PORTBbits.RB7 = PORTBbits.RB5 = PORTBbits.RB6 = PORTBbits.RB4 = 0;	//モーター動作停止
			__delay_ms(5);		//5msウェイト
		}
	}
	return 0;
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
