/*
 * File:   LAB03_DIGITAL2_SPI_MAIN.c
 * Author: Sergio Boch
 * Carnet: 20887
 * Created on August 17, 2022, 8:24 PM
 * I2C SLAVE
 */
//#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include "Oscilador.h" 
#include "ADC.h"  
#include "I2C.h"
#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>

#define _XTAL_FREQ 8000000

uint8_t contador = 0; 
uint8_t z;
uint8_t dato;
uint8_t val_adc;

void setup(void);

void __interrupt() isr (void){
    
    if (ADIF){
        if (ADCON0bits.CHS == 0)  
            val_adc = adc_read();
    }
    
    if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
        if (INTCONbits.T0IF){ // INTERRUPCION DEL TMR0
            tmr0_reload();
            cont--;
            if (cont == 0){
            cont1++;
            cont = 100; }
            if (cont1 ==16){
            cont1 = 0; }        
            PORTB = cont1;
           
   }
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL))
        {
            z = SSPBUF;             //Leemos el valor
            SSPCONbits.SSPOV = 0;   //Apagamos bandera de colision
            SSPCONbits.WCOL = 0;    
            SSPCONbits.CKP = 1;     
        }
        PIR1bits.SSPIF = 0;    
    }
    return;
}

void main(void) {     
    setup();
    while(1){  
        if(ADCON0bits.GO == 0){     // al no haber conversion
            __delay_us(50);
            ADCON0bits.GO = 1;      // se reinicia conversión
        }
        PORTB = val_adc;
        __delay_ms(5);
        }
}

void setup (void){
    ANSEL = 0b00000001;         //Entrada analogica
    ANSELH = 0b00000001;        // Usaremos solo I/O digitales  
    TRISA = 0b00000001;           
    TRISB = 0b00000000;                        
    TRISD = 0b00000000;

    PORTA = 0;
    PORTB = 0;
    PORTD = 0;

    I2C_Slave_Init(0x50); //Asignamos direccion al esclavo 0x50
      int_osc_MHz (3); //Config a 8MHz
      adc_init(1,0,0);   //  FOSC/32,Vdd,Vss
      adc_start(0);
       INTCONbits.PEIE = 1;        // Habilitamos interrupciones perifericas
       INTCONbits.GIE = 1;         // Habilitamos interrupciones globales  
    return;
}

