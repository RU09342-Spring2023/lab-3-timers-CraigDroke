/*
 * Part2.c
 *
 *  Created on: Feb 11, 2023
 *      Author: Russel Trafford and Craig Droke
 *
 *      This code will need to change the speed of an LED between 3 different speeds by pressing a button.
 */

#include <msp430.h>

void gpioInit();
void timerInit();

unsigned int count = 50000;                 //Variable to log count

void main(){

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM3_bits | GIE);

}


void gpioInit(){
    // Configure RED LED on P1.0 as Output
          P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
          P1DIR |= BIT0;                          // Set P1.0 to output direction

          // Configure Green LED on P6.6 as Output
          P6OUT &= ~BIT6;                         // Clear P6.6 output latch for a defined power-on state
          P6DIR |= BIT6;                          // Set P6.6 to output direction


          // Configure Button on P2.3 as input with pullup resistor
          P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
          P2REN |= BIT3;                          // P2.3 pull-up register enable
          P2IES &= ~BIT3;                         // P2.3 Low --> High edge
          P2IE |= BIT3;

}

void timerInit(){
    TB1CTL = TBSSEL_1 | MC_2 | TBCLR;           // ACLK, continuous mode, clear TAR
    TB1CCR0 = 50000;                          // Set CCR0 to toggle every 1/2 second
    TB1CCTL0 |= CCIE;                           // Enable CCR0 interrupt
}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    if (count == 50000)                   // starts with inital clock of 50000
       count = 20000;
    else if (count == 20000)            // speeds up the cycle to 20000 (when button pressed)
       count = 5000;
    else if (count == 5000)             // speeds up the cycle to 5000 (when button pressed again)
       count = 1000;
    else {
       count = 50000;                        // resets to initial clock of 50000 (when button pressed)
              }
    P2IFG &= ~BIT3;                     // clear P2.3 interrupt flag
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    P6OUT ^= BIT6;       // toggles green LED
    TB1CCR0 += count;        // offsets the clock
}


