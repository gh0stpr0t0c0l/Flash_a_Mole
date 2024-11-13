#include <p18cxxx.h>
#include "my_xlcd.h"
#include <delays.h>
#include <stdlib.h>

#pragma config FOSC = INTIO67, PLLCFG = OFF, PRICLKEN = ON,FCMEN = ON, PWRTEN = ON
#pragma config BOREN = SBORDIS, BORV = 250, WDTEN = OFF, WDTPS = 4096, PBADEN = OFF
#pragma config HFOFST = OFF, MCLRE = EXTMCLR, STVREN = ON, LVP = OFF, DEBUG = ON

unsigned long result = 0;
long int output;
unsigned int i;

void DelayFor18TCY( void ){
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
}

void DelayPORXLCD (void){
Delay1KTCYx(60); // Delay of 15ms
// Cycles = (TimeDelay * Fosc) / 4
// Cycles = (15ms * 16MHz) / 4
// Cycles = 60,000
return;
}

void DelayXLCD (void){
Delay1KTCYx(20); // Delay of 5ms
// Cycles = (TimeDelay * Fosc) / 4
// Cycles = (5ms * 16MHz) / 4
// Cycles = 20,000
return;
}

void Servo_4 (unsigned char value)
{
    CCPR4L = value >> 2;
    CCP4CON = CCP4CON|(value << 4);
}

void SERVO_UP(void) {
    Servo_4(93);
}

void SERVO_DN(void) {
    Servo_4(30);
}

void main( void ){  
//configure the Oscillator for 1Mhz internal oscillator operation
    OSCTUNEbits.PLLEN=0;
    OSCTUNEbits.INTSRC=0;
    OSCCON2 = 0x00; //no 4X PLL
    OSCCON = 0x30;  // 1MHZ //NEEDS TO BE 20
    //TRISC = 0x00;
    //ANSELC = 0x00;
    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01); //CLEAR DISPLAY
    WriteCmdXLCD(BLINK_ON); //BLINK ON
    WriteCmdXLCD(SHIFT_DISP_LEFT);
    //TRISA = 0xFF; 
    //ANSELA = 0x01;
    TRISA = 0x00; //XLCD DISPLAY
    ANSELA = 0x00;
    //VREFCON0 = 0xF0; //ADC STUFF
    //ADCON0 = 0b00000001;
    //ADCON1 = 0x08; //don't cares substituted for 0's
    //ADCON2 = 0b10110110;
    
    CCPTMRS0 = 0b00000000;
    PR2 = 0x9B;
    T2CON = 0b00000110;
    TRISD = 0x00;
    //CCP1CON = 0b00001100; //USED FOR OTHER MOTORS
    //CCP2CON = 0b00001100;
    //CCP3CON = 0b00001100;
    CCP4CON = 0b00001100;
    //CCP5CON = 0b00001100;
    TRISE = 0x00;
    
    while (1){
        SERVO_UP();
        Delay10KTCYx(5);
        SERVO_DN();
        Delay10KTCYx(5);
        
        //ADCON0bits.GO = 1;
        //while(ADCON0bits.GO);
        //result = ADRES;
        //result = result * 400000;

        SetDDRamAddr(0x00);
        putrsXLCD("Voltage is:");
        SetDDRamAddr(0x41);
        putcXLCD('.');
        SetDDRamAddr(0x47);
        putrsXLCD("Volts");
        
        //output = (result/100000%10000)/1000;
        //SetDDRamAddr(0x40);
        //putcXLCD(output+48);
        //for(i=1;i<=(output/0x01);++i){
        //    SetDDRamAddr(0x0B+0x01*i);
        //    putcXLCD(0xFF);
        //}
        //while ((0x0B + 0x01*i) <= 0x0F) {
        //    SetDDRamAddr(0x0B + 0x01*i);
        //    putcXLCD(0x20);
        //    i = i + 1;
        //}
        //output = (result/100000%1000)/100;
        //if ((output > 0) || (((result/100000%10000)/1000)>0)){
        //    SetDDRamAddr(0x0B);
        //    putcXLCD(0xFF);
        //}
        //else{
        //    SetDDRamAddr(0x0B);
        //    putcXLCD(0x20);
        //}
        //SetDDRamAddr(0x42);
        //putcXLCD(output+48);
        //output = (result/100000%100)/10;
        //SetDDRamAddr(0x43);
        //putcXLCD(output+48);
        //output = (result/100000%10);
        //SetDDRamAddr(0x44);
        //putcXLCD(output+48);
    }
}
