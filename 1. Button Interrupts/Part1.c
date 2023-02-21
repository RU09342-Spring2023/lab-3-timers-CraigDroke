/*
 * Part1.c
 *
 *  Created on: Feb 11, 2023
 *      Author: Russel Trafford and Craig Droke
 *
 *  This code is a template which will change the color of the LED being blinked using the interrupt routine.
 */


#include <msp430.h>

char LED_Color = 0;                       // Global Variable to determine which LED should be blinking
char edge = 1;                            // Variable to track the edge of flag

void gpioInit();


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();



    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    P2IFG &= ~BIT3;                         // P2.3 IFG cleared

    __bis_SR_register(GIE);                 // Enter LPM3 w/interrupt

    while(1)
    {
        if (LED_Color == 1) {
            P1OUT ^= BIT0;                  // P1.0 = toggle
            P6OUT &= ~BIT6;                 // Clear P6.6
            __delay_cycles(100000);         // Delay for 1 second
        }

        else{
            P6OUT ^= BIT6;                 // Set P1.0 to 0
            P1OUT &= ~BIT0;                 // Clear P1.0
            __delay_cycles(100000);         //Delay for 1 second
            }
        }

}

/*
 * gpioInit()
 *
 * Configures the following GPIO Pins:
 *  - P1.0: Output
 *  - P6.6: Output
 *  - P2.3: Input with Pullup Resistor
 */

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
      P2IE |= BIT3;                           // P2.3 interrupt enabled

}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

    if ((P2IFG & BIT3) && (edge == 1))   //If the flag is triggered and the edge is high
    {
        LED_Color = 1;                  // Will set the LED color to red
        P2IES &= ~BIT3;                 // Set the edge select to low
        edge = 0;                       // Tracks edge select low
        P2IFG &= ~BIT3;                 // Clear the int flag
    }

    else if ((P2IFG & BIT3) && (edge == 0))  //If the flag is triggered and the edge is low
    {
        LED_Color = 0;                       // Will set the LED color to green
        P2IES |= BIT3;                       // Set the edge select to high
        edge = 1;                            // Tracks edge select high
        P2IFG &= ~BIT3;                      // Clear the int flag
    }
}

