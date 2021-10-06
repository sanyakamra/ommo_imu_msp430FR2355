//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  Texas Instruments, Inc
//  July 2013
//***************************************************************************************

#include <msp430.h>

int flag = 0;
int rx_data;
void main(void) {
  int write = 0b00000000;
  int read = 0b10000000;
  int i = 0;
  int j;

  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
  UCA0CTLW0 |= UCSWRST; //PUT A0 INTO SW RESET
  UCA0CTLW0 |= UCSSEL__SMCLK; //CHOOSE SMCLK
  UCA0BRW = 10; //SET PRESACALAR TO 10 TO GET SCLK=10KHz
  UCA0CTLW0 |= UCSYNC; // PUT A0 INTO SPI MODE
  UCA0CTLW0 |= UCMST; // PUT INTO SPI MASTER
  // CONFIGURE PORTS
  P4DIR &= ~BIT1; // ENABLE P4.1 IRQ
  P4REN |= BIT1; //ENABLE RESISTOR
  P4OUT |= BIT1; //MAKE RESISTOR TO PULL UP
  P4IES |= BIT1; //SENSITIVE TO H TO L
  P1SEL1 &= ~BIT5; // P1.5 = SCLK (P1SEL1: P1SEL0 = 01)
  P1SEL0 |= BIT5;
  P1SEL1 &= ~BIT7; // P1.7 = SIMO (P1SEL1: P1SEL0 = 01)
  P1SEL0 |= BIT7;
  P1SEL1 &= ~BIT6; // P1.6 = SOMI (P1SEL1: P1SEL0 = 01)
  P1SEL0 |= BIT6;
  UCA0CTLW0 |= UCSWRST; // TAKE OUT OF SW RESET
  //ENABLE IRQ
  //    P4IE|= BIT1 ;//ENABLE P4.1 IRQ
  //    P4IFG&= ~ BIT1 ;//CLEAR FLAG
  //Configuration for registers of imu
  int address_conf[] = {
    0x0A,
    0x12,
    0x10,
    0x11
  };
  int conf[] = {
    0b00111110,
    0b00001000,
    0b01110100,
    0b01111110
  };

  //Tx the configuration to IMU(send/write)

  for (i = 0; i < 4; i++) {

    while (!(UCTXCPTIFG & UCA0IFG)); //the flag is set when TX is ready
    UCA0TXBUF = write | address_conf[i];
    while ((UCA0STATW & UCBUSY)); // UCBUSY indicates when rx/tx is in progress
    UCA0TXBUF = conf[i];

  }
  //Read the data of specific sensor(Acc/gyro)
  /*
   OUTX_L_G (22h) & OUTX_H_G (23h)
          Angular rate sensor pitch axis (X) angular rate output register (r)  -CR CTRL2_G (11h))
   OUTY_L_G (24h) & OUTY_H_G (25h)
          Angular rate sensor roll axis (Y) angular rate output register (r)   -CR CTRL2_G (11h))
   OUTZ_L_G (26h) & OUTZ_H_G (27h)
          Angular rate sensor yaw axis (Z) angular rate output register (r)   -CR CTRL2_G (11h))

   OUTX_L_XL (28h) & OUTX_H_XL (29h)
          Linear acceleration sensor X-axis output register (r)
   OUTY_L_XL (2Ah) & OUTY_H_XL (2Bh)
          Linear acceleration sensor Y-axis output register (r)
   OUTZ_L_XL (2Ch) & OUTZ_H_XL (2Dh)
          Linear acceleration sensor Z-axis output register (r)

   */

  __enable_interrupt();
  int address_info[] = {
    0x23,
    0x22,
    0x25,
    0x24,
    0x27,
    0x26,
    0x29,
    0x28,
    0x2B,
    0x2A,
    0x2D,
    0x2C
  }; // output of gyro and acc
  while (1) {
    if (flag == 1) {
      for (j = 0; j < 12; j++) {
        while (!(UCTXCPTIFG & UCA0IFG));
        UCA0TXBUF = read | address_info[j];
        while ((UCA0STATW & UCBUSY));

      }
    }
    if (flag == 0 && ~(P4IN & BIT1)) {

      UCA0IE |= UCRXIE; //ENABLE A0 RX IRQ
      UCA0IE |= UCTXIE; //ENABLE A0 TX IRQ
      UCA0IFG &= ~UCTXIFG; //CLEAR FLAG
      UCA0IFG &= ~UCRXIFG; //CLEAR FLAG
      flag = 1;

    } else if (flag == 1 && ~(P4IN & BIT1)) {
      UCA0IE &= ~UCRXIE; //DISABLE A0 RX IRQ
      UCA0IE &= ~UCTXIE; //DISABLE A0 TX IRQ
      __delay_cycles(500000); ///  enable timer for 500ms MCLK=1Mhz
      if (~(P4IN & BIT1)) { // check if button is still pressed
        UCA0IE &= ~UCRXIE; //DISABLE A0 RX IRQ
        UCA0IE &= ~UCTXIE; //DISABLE A0 TX IRQ
        flag = 0;
      } else {
        UCA0IE |= UCRXIE; //ENABLE A0 RX IRQ
        UCA0IE |= UCTXIE; //ENABLE A0 TX IRQ
        UCA0IFG &= ~UCTXIFG; //CLEAR FLAG
        UCA0IFG &= ~UCRXIFG; //CLEAR FLAG
      }

      // ENABLE GLOBAL INTERRUPT
      //flag=0 and assuming device is at OFF state

    }
  }
}

//__interrupt void ISR_port4_s1(void) {
//    if (flag ==0) {
//        //////  Enable timer for 200ms
//         // check if button is still pressed
//            UCA0IE |= UCRXIE; //ENABLE A0 RX IRQ
//            UCA0IE |= UCTXIE; //ENABLE A0 TX IRQ
//            UCA0IFG&= ~ UCTXIFG; //CLEAR FLAG
//            UCA0IFG&= ~ UCRXIFG; //CLEAR FLAG
//            flag=1;
//
//    }
//    else {
//        __delay_cycles(500000); ///  enable timer for 500ms
//        if( ~( P4IN & BIT1 ) )
//        { // check if button is still pressed
//            UCA0IE &= ~UCRXIE; //DISABLE A0 RX IRQ
//            UCA0IE &= ~ UCTXIE; //DISABLE A0 TX IRQ
//            flag=0;
//        }
//        P4IFG&= ~ BIT1; //CLEAR FLAAG
//    }
//}

//Enable interrupt for Rx
#pragma vector = EUSCI_A0_VECTOR //data in A0 spi buffer
__interrupt void ISR_EUSCI_A0(void) {

  rx_data = UCA0RXBUF; //here I am not collecting data of different register of gyro ans acc. just dummy data collection
}
