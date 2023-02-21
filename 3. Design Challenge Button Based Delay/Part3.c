/*
 * Part3.c
 *
 *  Created on: Feb 14, 2023
 *      Author: Craig Droke
 *
 *      The purpose of this is to have an LED blink differently based off of how long you hold down the button. The
 *      other button resets.
 */
#include <msp430.h>

void gpioInit();
void timerInit();

unsigned int blinktime = 50000;             // Variable to log the time to blink
int buttonon = 0;                           // Binary varaible to track if the button is held up or down
long previoustime = 0;                      // Variable to track the previous time logged

void main(){

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();

    if(!(P4IN & BIT1)) {
        blinktime = 50000;                  // Reset button pressed, revert to default blink time
    }

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

    // Configure Reset Button on P4.1 as input with pullup resistor
    P4OUT |= BIT1;                          // Configure P4.1 as pulled-up
    P4REN |= BIT1;                          // P4.1 pull-up register enable
    P4IES &= ~BIT1;                         // P4.1 Low --> High edge
    P4IE |= BIT1;

}

void timerInit(){
    TB1CTL = TBSSEL_1 | MC_2 | TBCLR;           // ACLK, continuous mode, clear TAR
    TB1CCR0 = 100000;                           // Set CCR0 to toggle every 1 second
    TB1CCTL0 |= CCIE;                           // Enable CCR0 interrupt
}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    if (P2IN & BIT3) { // button released
        if (buttonon) {

            long presstime = TB1R - previoustime;   // calculate the duration of button press
            blinktime = presstime;                  // set the blink period to the duration of button press
            buttonon = 0;                           // reset the button press flag
            previoustime = 0;                       // Reset the previous time recorded
        }
    } else { // button pressed down
        buttonon = 1;              // signal that button is in pressed state
        previoustime = TB1R;       // record the time of button press
    }
    P2IFG &= ~BIT3;                // Reset int flag
}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    if (P4IN & BIT1) {             // reset button released
        blinktime = 50000;         // Reset to default blink time
    }
    P4IFG &= ~BIT1;                // Reset the int flag
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    P6OUT ^= BIT6;                // toggles green LED
    TB1CCR0 += blinktime;         // offsets the clock
}
