#include <p18cxxx.h>
#include "my_xlcd.h"
#include <delays.h>
#include <stdlib.h>

#pragma config FOSC = INTIO67, PLLCFG = OFF, PRICLKEN = ON,FCMEN = ON, PWRTEN = ON
#pragma config BOREN = SBORDIS, BORV = 250, WDTEN = OFF, WDTPS = 4096, PBADEN = OFF
#pragma config HFOFST = OFF, MCLRE = EXTMCLR, STVREN = ON, LVP = OFF, DEBUG = ON
#pragma config CCP3MX = PORTE0

unsigned long result = 0;
int newRand = 0;
unsigned char buttonResult = 0;
unsigned char button1 = 1;
unsigned char button2 = 1;
unsigned long newLongRand = 0;
unsigned long photoAvg1 = 0;
unsigned long photoAvg2 = 0;
unsigned long photoAvg3 = 0;
unsigned long photoAvg4 = 0;
unsigned long timestamp1 = 0;
unsigned long timestamp2 = 0;
unsigned long timestamp3 = 0;
unsigned long timestamp4 = 0;
unsigned long timestampTimer = 0;
unsigned char state1 = 0;
unsigned char state2 = 0;
unsigned char state3 = 0;
unsigned char state4 = 0;
char timer1 = 60;
char timer2 = 60;
char timer3 = 60;
char timer4 = 60;
char timer5 = 60;
unsigned long avgSum = 0;
long int output;
unsigned int i;
unsigned char TOP = 93;
unsigned char BOTTOM = 30; 
unsigned char j;
char my_buff[3];
char state = 0;
unsigned char score = 0;
unsigned long counter = 0;
rom unsigned char modeMatrix [5][8] = {" the OG", " bomber", "  mode3", "  mode4", "  mode5"}; //size 7 including space at front
unsigned char modeMatrixi = 0;
unsigned char genericTimer = 0;


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

int myRand(unsigned char bound){
    newRand = ADRES;
    newRand = ((newRand+rand()) % bound) + 1;
    return newRand;
}

unsigned long longRand(void){
    newLongRand = myRand(5)+1;
    newLongRand = newLongRand * 0x55; //was 3FFF
    return newLongRand;
}

unsigned long readPhoto(unsigned char photoNum)
{
    switch (photoNum){
        case 1:
            ADCON0 = 0b00111101;
            break;
        case 2:
            ADCON0 = 0b01000001;
            break;
        case 3:
            ADCON0 = 0b01000101;
            break;
        case 4:
            ADCON0 = 0b01001001;
            break;
    }
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);
    result = ADRES;
    result = result * 400000;

    return result;
}

unsigned long takePhotoAvg(unsigned char photoNum)
{
    switch (photoNum){
        case 1:
            ADCON0 = 0b00111101;
            break;
        case 2:
            ADCON0 = 0b01000001;
            break;
        case 3:
            ADCON0 = 0b01000101;
            break;
        case 4:
            ADCON0 = 0b01001001;
            break;
    }
    avgSum = 0;
    for(j=0;j<10;++j){
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO);
        result = ADRES;
        result = result * 400000;
        avgSum += result;
    }
    
    return avgSum/10;
}

void setState(unsigned char servoOpt, unsigned char stateOpt){
    if (stateOpt == TOP) {
        i = 1;
    }
    else if (stateOpt == BOTTOM) {
        i = 0;
    }
    switch (servoOpt){
        case 1:
            state1 = i;
            break;
        case 2:
            state2 = i;
            break;
        case 3:
            state3 = i;
            break;
        case 4:
            state4 = i;
            break;
    }
}

void Servo_2 (unsigned char value)
{
    setState(2,value);
    CCPR1L = value >> 2;
    value = value & 0x03;
    CCP1CON = CCP1CON|(value << 4);
}

void Servo_3 (unsigned char value)
{
    setState(3,value);
    CCPR2L = value >> 2;
    value = value & 0x03;
    CCP2CON = CCP2CON|(value << 4);
}

void Servo_1 (unsigned char value)
{
    setState(1,value);
    CCPR4L = value >> 2;
    value = value & 0x03;
    CCP4CON = CCP4CON|(value << 4);
}

void Servo_4 (unsigned char value)
{
    setState(4,value);
    CCPR5L = value >> 2;
    value = value & 0x03;
    CCP5CON = CCP5CON|(value << 4);
}

void ledOn(unsigned char ledSelect, unsigned char ledValue){
    switch (ledSelect) {
        case 1:
            PORTBbits.RB1 = ledValue;
            break;
        case 2:
            PORTBbits.RB2 = ledValue;
            break;
        case 3:
            PORTBbits.RB3 = ledValue;
            break;
        case 4:
            PORTBbits.RB4 = ledValue;
            break;
    }        
}

unsigned char checkButton(unsigned char buttonSelect){
    buttonResult = 0;
    switch (buttonSelect){
        case 1:
            if(!PORTDbits.RD2){    //check key 1
                Delay1KTCYx(3);      //delay for debounce;
                if(!PORTDbits.RD2) {
                    while(!PORTDbits.RD2);
                    buttonResult = 1;
                }
            }
            break;
        case 2:
            if(!PORTDbits.RD3){    //check key 2
                Delay1KTCYx(3);      //delay for debounce;
                if(!PORTDbits.RD3) {
                    while(!PORTDbits.RD3);
                    buttonResult = 1;
                }
            }
            break;
    }
    return buttonResult;
}

void printScore(void){
    SetDDRamAddr(0x02);
    putrsXLCD("Score: ");
    if (score < 10) {
        putrsXLCD("0");
    }
    itoa(score, my_buff);
    putsXLCD(my_buff); 
}

void calibrate(void){
    Servo_1(BOTTOM); //CHANGED to BOTTOM from TOP
    Servo_2(BOTTOM); //CHANGED to BOTTOM from TOP
    Servo_3(BOTTOM); //CHANGED to BOTTOM from TOP
    Servo_4(BOTTOM); //CHANGED to BOTTOM from TOP
    photoAvg1 = takePhotoAvg(1);
    photoAvg2 = takePhotoAvg(2);
    photoAvg3 = takePhotoAvg(3);
    photoAvg4 = takePhotoAvg(4);
}

void hit(unsigned char servoNum) { //if state ==7 make sure to turn light off
    switch (servoNum) {
        case 1:
            if (state1 == 1){
                if (state != 7){
                    if (state == 6){
                        ledOn(1,1);
                    }
                    score = score + 1;
                    timestamp1 = counter + longRand();
                    printScore();
                }
                else {
                    if(PORTBbits.RB1 == 1){
                        ledOn(1,0);
                        score = score + 1;
                        timestamp1 = counter + longRand();
                        printScore();
                    }
                    else{
                        if (score > 0){
                            score = score - 1;
                        }
                        timestamp1 = counter + longRand();
                        printScore();
                    }
                }
            }
            break;
        case 2:
            if (state2 == 1){
                if (state != 7){
                    if (state == 6){
                        ledOn(2,1);
                    }
                    score = score + 1;
                    timestamp2 = counter + longRand();
                    printScore();
                }
                else {
                    if(PORTBbits.RB2 == 1){
                        ledOn(2,0);
                        score = score + 1;
                        timestamp2 = counter + longRand();
                        printScore();
                    }
                    else{
                        if (score > 0){
                            score = score - 1;
                        }
                        timestamp2 = counter + longRand();
                        printScore();
                    }
                }
            }
            break;
        case 3:
            if (state3 == 1){
                if (state != 7){
                    if (state == 6){
                        ledOn(3,1);
                    }
                    score = score + 1;
                    timestamp3 = counter + longRand();
                    printScore();
                }
                else {
                    if(PORTBbits.RB3 == 1){
                        ledOn(3,0);
                        score = score + 1;
                        timestamp3 = counter + longRand();
                        printScore();
                    }
                    else{
                        if (score > 0){
                            score = score - 1;
                        }
                        timestamp3 = counter + longRand();
                        printScore();
                    }
                }
            }
            break;
        case 4:
            if (state4 == 1){
                if (state != 7){
                    if (state == 6){
                        ledOn(4,1);
                    }
                    score = score + 1;
                    timestamp4 = counter + longRand();
                    printScore();
                }
                else {
                    if(PORTBbits.RB4 == 1){
                        ledOn(4,0);
                        score = score + 1;
                        timestamp4 = counter + longRand();
                        printScore();
                    }
                    else{
                        if (score > 0){
                            score = score - 1;
                        }
                        timestamp4 = counter + longRand();
                        printScore();
                    }
                }
            }
            break;
    }  
}

void printCorners(void){
    SetDDRamAddr(0x00);
    putcXLCD(0xDB);
    SetDDRamAddr(0x40);
    putcXLCD(0xDB);
    SetDDRamAddr(0x0F);
    putcXLCD(0xDB);
    SetDDRamAddr(0x4F);
    putcXLCD(0xDB);
}

void updateTimer(unsigned char timerSelect){
    if (counter >= timestampTimer) {
        genericTimer = genericTimer + 1;
        timestampTimer = counter + 85; //used in 2 places - update both
        SetDDRamAddr(0x42);
        switch (timerSelect){
            case 1:
                putcXLCD(((timer1 - genericTimer)%100)/10 + 48);
                putcXLCD(((timer1-genericTimer)%10) + 48);
                if ((timer1 - genericTimer) == 0xFF){
                    if (state == 7) {
                        PORTBbits.RB1 = 0;
                        PORTBbits.RB2 = 0;
                        PORTBbits.RB3 = 0;
                        PORTBbits.RB4 = 0;
                    }
                    state = 11;
                }
                break;
            case 2:
                putcXLCD(((timer2 - genericTimer)%100)/10 + 48);
                putcXLCD(((timer2-genericTimer)%10) + 48);
                if ((timer2 - genericTimer) == 0xFF){
                    if (state == 7) {
                        PORTBbits.RB1 = 0;
                        PORTBbits.RB2 = 0;
                        PORTBbits.RB3 = 0;
                        PORTBbits.RB4 = 0;
                    }
                    state = 11; 
                }
                break;
            case 3:
                putcXLCD(((timer3 - genericTimer)%100)/10 + 48);
                putcXLCD(((timer3-genericTimer)%10) + 48);
                if ((timer3 - genericTimer) == 0xFF){
                    if (state == 7) {
                        PORTBbits.RB1 = 0;
                        PORTBbits.RB2 = 0;
                        PORTBbits.RB3 = 0;
                        PORTBbits.RB4 = 0;
                    }
                    state = 11;
                }
                break;
            case 4:
                putcXLCD(((timer4 - genericTimer)%100)/10 + 48);
                putcXLCD(((timer4-genericTimer)%10) + 48);
                if ((timer4 - genericTimer) == 0xFF){
                    if (state == 7) {
                        PORTBbits.RB1 = 0;
                        PORTBbits.RB2 = 0;
                        PORTBbits.RB3 = 0;
                        PORTBbits.RB4 = 0;
                    }
                    state = 11;
                }
                break;
            case 5:
                putcXLCD(((timer5 - genericTimer)%100)/10 + 48);
                putcXLCD(((timer5-genericTimer)%10) + 48);
                if ((timer5 - genericTimer) == 0xFF){
                    if (state == 7) {
                        PORTBbits.RB1 = 0;
                        PORTBbits.RB2 = 0;
                        PORTBbits.RB3 = 0;
                        PORTBbits.RB4 = 0;
                    }
                    state = 11;
                }
                break; 
        }
    }
}

void main( void ){  
//configure the Oscillator for 1Mhz internal oscillator operation
    OSCTUNEbits.PLLEN=0;
    OSCTUNEbits.INTSRC=0;
    OSCCON2 = 0x00; //no 4X PLL
    OSCCON = 0x30;  // 1MHZ //NEEDS TO BE 20 30 for LCD
    TRISA = 0x00; //XLCD DISPLAY
    ANSELA = 0x00;
    ANSELB = 0x00;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    ledOn(1,0);
    ledOn(2,0);
    ledOn(3,0);
    ledOn(4,0);
    
    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01); //CLEAR DISPLAY
    WriteCmdXLCD(BLINK_ON); //BLINK ON
    WriteCmdXLCD(SHIFT_DISP_LEFT);
    VREFCON0 = 0xF0; //ADC STUFF
    //ADCON0 = 0b00111101;
    ADCON1 = 0x08; //don't cares substituted for 0's
    ADCON2 = 0b10110110;
    
    CCPTMRS0 = 0x00; //was 0x00 --added
    CCPTMRS1 = 0x00;
    PR2 = 0x9B; //timer 2 config
    //PR4 = 0x9B; //timer 4 config --added
    T2CON = 0b00000110; //timer 2 config
    
    TRISC = 0xF8; //servo 1, 2; ADC 1
    ANSELC = 0xF8; 
    TRISD = 0x0C; //servo 4, push buttons
    ANSELD = 0x00;
    TRISE = 0x00; //servo 3, 5
    ANSELE = 0x00;
    
    CCP1CON = 0b00001100; //USED FOR OTHER MOTORS
    CCP2CON = 0b00001100;
    CCP3CON = 0b00001100;
    CCP4CON = 0b00001100;
    CCP5CON = 0b00001100;
    
    while (1){
        if (state == 0){ //INIT
            //calibrate(); //need time to calibrate CHANGED: line removed
            Servo_1(TOP); //CHANGED: added line
            Servo_2(TOP); //CHANGED: added line
            Servo_3(TOP); //CHANGED: added line
            Servo_4(TOP); //CHANGED: added line
            printCorners();
            SetDDRamAddr(0x01);
            putrsXLCD(" Flash-a-Mole ");
            SetDDRamAddr(0x41);
            putrsXLCD(" Start");
            //Delay10KTCYx(25);
            
            state = 1; //skip mode select (state 1) to start game
        }
        
        if (state == 1)
        {
            if (checkButton(1) == 1) {
                state = 2;
            }
            if (checkButton(2) == 1) {
                SetDDRamAddr(0x01);
                putrsXLCD(" SPOON KILLER ");
            }
        }
        
        if (state == 2) {
            WriteCmdXLCD(0x01); //CLEAR DISPLAY
            //calibrate(); //CHANGED: line removed
            while(BusyXLCD());
            printCorners();
            SetDDRamAddr(0x02);
            putrsXLCD("Mode:");
            SetDDRamAddr(0x42);
            putrsXLCD("Start   Next");
            state = 3;
        }
        
        if (state == 3){
            SetDDRamAddr(0x07);
            putrsXLCD(modeMatrix[modeMatrixi]);
            state = 4;
        }
        
        if (state == 4){
            if (checkButton(2) == 1) {
                ledOn(1,1);
                Delay1KTCYx(5);
                ledOn(1,0);
                ledOn(2,1);
                Delay1KTCYx(5);
                ledOn(2,0);
                ledOn(3,1);
                Delay1KTCYx(5);
                ledOn(3,0);
                ledOn(4,1);
                Delay1KTCYx(5);
                ledOn(4,0);
                modeMatrixi = modeMatrixi + 1;
                if (modeMatrixi > 4){
                    modeMatrixi = 0;
                }
                state = 3;
            }
            if (checkButton(1) == 1) {
                genericTimer = 0;
                state = modeMatrixi+6;
                timestamp1 = counter + longRand();
                timestamp2 = counter + longRand();
                timestamp3 = counter + longRand();
                timestamp4 = counter + longRand();
                WriteCmdXLCD(0x01); //CLEAR DISPLAY
                calibrate();
                while(BusyXLCD());
                SetDDRamAddr(0x03);
                putrsXLCD("Get Ready");
                Delay10KTCYx(50);
                SetDDRamAddr(0x46);
                putrsXLCD("3...");
                Delay10KTCYx(35);
                SetDDRamAddr(0x46);
                putrsXLCD("2");
                Delay10KTCYx(35);
                SetDDRamAddr(0x46);
                putrsXLCD("1");
                Delay10KTCYx(35);
                SetDDRamAddr(0x46);
                putrsXLCD("GO!!");
                Delay10KTCYx(35);
                Servo_1(BOTTOM);
                Servo_2(BOTTOM);
                Servo_3(BOTTOM);
                Servo_4(BOTTOM);
                timer1 = 60; //set these in 2 places
                timer2 = 60;
                timer3 = 60;
                timer4 = 60;
                timer5 = 60;
                WriteCmdXLCD(0x01); //CLEAR DISPLAY
                while(BusyXLCD());
                printCorners();
                SetDDRamAddr(0x02);
                putrsXLCD("Score: 0");
                score = 0;
                SetDDRamAddr(0x47);
                putrsXLCD(modeMatrix[modeMatrixi]);
                timestampTimer = counter + 85;
            }
        }
        
        if (state == 6){ //the OG
            updateTimer(1);
            if (checkButton(1) == 1) {
                state = 2;
                modeMatrixi = 0;
            }
            if (checkButton(2) == 1) {
                state = 2;
                modeMatrixi = 0;
            }
            ledOn(1,0);
            ledOn(2,0);
            ledOn(3,0);
            ledOn(4,0);
            
            if (readPhoto(1) > 3*photoAvg1) {
                hit(1);
                Servo_1(BOTTOM);
            }
            if (readPhoto(2) > 3*photoAvg2) {
                hit(2);
                Servo_2(BOTTOM);
            }
            if (readPhoto(3) > 3*photoAvg3) {
                hit(3);
                Servo_3(BOTTOM);
            }
            if (readPhoto(4) > 3*photoAvg4) {
                hit(4);
                Servo_4(BOTTOM);
            }
            
            if (counter > timestamp1) {
                switch (state1) {
                    case 0:
                        Servo_1(TOP);
                        break;
                    case 1:
                        Servo_1(BOTTOM);
                        break;
                }
                timestamp1 = counter + longRand();
            }
            
            if (counter > timestamp2) {
                switch (state2) {
                    case 0:
                        Servo_2(TOP);
                        break;
                    case 1:
                        Servo_2(BOTTOM);
                        break;
                }
                timestamp2 = counter + longRand();
            }
            
            if (counter > timestamp3) {
                switch (state3) {
                    case 0:
                        Servo_3(TOP);
                        break;
                    case 1:
                        Servo_3(BOTTOM);
                        break;
                }
                timestamp3 = counter + longRand();
            }
            
            if (counter > timestamp4) {
                switch (state4) {
                    case 0:
                        Servo_4(TOP);
                        break;
                    case 1:
                        Servo_4(BOTTOM);
                        break;
                }
                timestamp4 = counter + longRand();
            }
            
            counter = counter+1;
        }
        
        if (state == 7){ 
            updateTimer(2);
            if (checkButton(1) == 1) {
                state = 2;
                modeMatrixi = 0;
                PORTBbits.RB1 = 0;
                PORTBbits.RB2 = 0;
                PORTBbits.RB3 = 0;
                PORTBbits.RB4 = 0;
            }
            if (checkButton(2) == 1) {
                state = 2;
                modeMatrixi = 0;
                PORTBbits.RB1 = 0;
                PORTBbits.RB2 = 0;
                PORTBbits.RB3 = 0;
                PORTBbits.RB4 = 0;
            }
            
            if (readPhoto(1) > 3*photoAvg1) {
                hit(1);
                Servo_1(BOTTOM);
            }
            if (readPhoto(2) > 3*photoAvg2) {
                hit(2);
                Servo_2(BOTTOM);
            }
            if (readPhoto(3) > 3*photoAvg3) {
                hit(3);
                Servo_3(BOTTOM);
            }
            if (readPhoto(4) > 3*photoAvg4) {
                hit(4);
                Servo_4(BOTTOM);
            }
            
            if (counter > timestamp1) {
                switch (state1) {
                    case 0:
                        Servo_1(TOP);
                        ledOn(1,(myRand(2)-1));
                        break;
                    case 1:
                        Servo_1(BOTTOM);
                        ledOn(1,0);
                        break;
                }
                timestamp1 = counter + longRand();
            }
            
            if (counter > timestamp2) {
                switch (state2) {
                    case 0:
                        Servo_2(TOP);
                        ledOn(2,(myRand(2)-1));
                        break;
                    case 1:
                        Servo_2(BOTTOM);
                        ledOn(2,0);
                        break;
                }
                timestamp2 = counter + longRand();
            }
            
            if (counter > timestamp3) {
                switch (state3) {
                    case 0:
                        Servo_3(TOP);
                        ledOn(3,(myRand(2)-1));
                        break;
                    case 1:
                        Servo_3(BOTTOM);
                        ledOn(3,0);
                        break;
                }
                timestamp3 = counter + longRand();
            }
            
            if (counter > timestamp4) {
                switch (state4) {
                    case 0:
                        Servo_4(TOP);
                        ledOn(4,(myRand(2)-1));
                        break;
                    case 1:
                        Servo_4(BOTTOM);
                        ledOn(4,0);
                        break;
                }
                timestamp4 = counter + longRand();
            }
            counter = counter+1;
        }
        
        if (state == 11) {
            SetDDRamAddr(0x01);
            putrsXLCD("  TIME'S UP!  ");
            SetDDRamAddr(0x41);
            putrsXLCD("  SCORE: ");
            if (score < 100) {
                putrsXLCD("0");
            }
            if (score < 10) {
                putrsXLCD("0");
            }
            itoa(score, my_buff);
            putsXLCD(my_buff);
            putrsXLCD("  ");
            if (checkButton(1) == 1) {
                state = 2;
                modeMatrixi = 0;
            }
            if (checkButton(2) == 1) {
                state = 2;
                modeMatrixi = 0;
            }
        }
    }
}
