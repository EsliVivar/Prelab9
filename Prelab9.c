/* 
 * File:   Prelab9.c
 * Author: Esli Vivar.
 *
 *Descripcion: Utilizando un canal analógico realice la conversión ADC y configure el módulo
PWM 1 del microcontrolador para poder generar una señal de PWM que controle
todo el rango de movimiento de un servo. 
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>  // Para poder usar los int de 8 bits
/*
 * Constantes
 */
//El valor del oscilador de 800khZ
#define _XTAL_FREQ 8000000
/*
 * Variables 
 */

/*
 * Prototipos de funciones
 */
void setup(void);

/*
 * Interrupcion 
 */
void __interrupt() isr(void){
    if(PIR1bits.ADIF){
        CCPR1L = (ADRESH>>1)+124;
        CCP1CONbits.DC1B1 = ADRESH & 0b01;
        CCP1CONbits.DC1B0 = (ADRESL>>7);
        PIR1bits.ADIF = 0;
       
    }
}

/*
 * Codigo Principal
 */
void main(void){
    setup();
    while(1){
        if(ADCON0bits.GO == 0){
            __delay_us(50);
            ADCON0bits.GO = 1;
        }
    }
}
/*
 * Funciones
 */
void setup(void){
    ANSEL = 0b00000001;
    ANSELH = 0;
    
    TRISA = 1;
    
    OSCCONbits.IRCF = 0b0111; // Oscilador a 8MHz
    OSCCONbits.SCS = 1;
    
    ADCON1bits.ADFM = 0; //Justificado a la izquierda
    ADCON1bits.VCFG0 = 0; // Voltajes de referencia VSS y VDD
    ADCON1bits.VCFG1 = 0; 
    
    ADCON0bits.ADCS = 0b10; //Fosc/32
    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;
    __delay_us(50);
    
    TRISCbits.TRISC2 = 1;       // RC2 como entrada
    PR2 = 255;                  // Configuracion del periodo 
    CCP1CONbits.P1M = 0;        // Configuracion de PWM
    CCP1CONbits.CCP1M = 0b1100;
    
    CCPR1L = 0x0f;              // Primer ciclo de trabajo
    CCP1CONbits.DC1B = 0;
    
    PIR1bits.TMR2IF = 0;        // Bandera apagada 
    T2CONbits.T2CKPS = 0b11;    // Preescalador 1:16
    T2CONbits.TMR2ON =1;
    
    while(PIR1bits.TMR2IF == 0);    // Espera de un ciclo de tmr2
    PIR1bits.TMR2IF = 0;
    
    TRISCbits.TRISC2 = 0;   // Salida de PWM
    
    PIR1bits.ADIF = 0;  // Configuración de interrupciones
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    return;
}
