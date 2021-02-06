
#include "LCD.h"
#include <stdio.h>
#include <stdlib.h>
//
// Function Declarations
//
void initBasicTimer(void);
void initPortPins(void);

int sw1=0,sw2=0;
int stopflag;
#define SW1 BIT0&P1IN
#define SW2 BIT1&P1IN
 int t, t1,t2,t3;
char gm1[] = "Menu\n\r1)Enable StopWatch\n\r2)Timer\n\r3)Disable Stopwatch\n\r";
char gm2[] = "\n\rStop watch selected\n\rEnabling stopwatch on board\n\r";
char gm3[]=  "\n\rTimer selected enter the time in MM:SS format to set the timer\n\r";
char gm4[]=  "\n\rDisabling Stopwatch\n\r";
char mes[]=  "Timer selected to            \n\r";
unsigned char ch;                   // hold char from UART RX
unsigned char rx_flag;              // receiver rx status flag

void SPISetup(void)
{
	UCB0CTL0 = UCMSB + UCMST + UCSYNC; 	// sync. mode, 3-pin SPI, Master mode, 8-bit data
	UCB0CTL1 = UCSSEL_2 + UCSWRST;   // SMCLK and Software reset                  
	UCB0BR0 = 0x02; 				      // Data rate = SMCLK/2 ~= 500kHz                          
	UCB0BR1 = 0x00;
	P3SEL |= BIT1 + BIT2 + BIT3;	   // P3.1,P3.2,P3.3 option select               
	UCB0CTL1 &= ~UCSWRST; 			   // **Initialize USCI state machine**
}

void UART0_putchar(char c) 
{
	// wait for other character to transmit
	while (!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = c;
}
void Serial_Initialize(void)
{
	P2SEL |= BIT4+BIT5; 		         // Set UC0TXD and UC0RXD to transmit and receive data
	UCA0CTL1 |= BIT0; 			      // Software reset
	UCA0CTL0 = 0; 				         // USCI_A0 control register
	UCA0CTL1 |= UCSSEL_2; 		      // Clock source SMCLK - 1048576 Hz
	UCA0BR0=54; 				         // baud rate - 1048576 Hz / 19200
	UCA0BR1=0; 					         //
	UCA0MCTL=0x0A; 				      // Modulation
	UCA0CTL1 &= ~BIT0; 			      // Software reset
	IE2 |=UCA0RXIE;				      // Enable USCI_A0 RX interrupt
}

void main(void)
{
  WDTCTL = WDT_MDLY_0_5;
  FLL_CTL0 |= XCAP18PF;                     // Set load cap for 32k xtal
  Serial_Initialize();
  SPISetup();
  
  _EINT();					            // Enable global interrupts

  for (int z = 100; z > 0; z--);   // Delay to allow baud rate stabilize

  // Greeting Message
  for(int i = 0; i < 55; i++) {
          ch = gm1[i];
          UART0_putchar(ch);		      // print the greeting message on hyperterminal
  }

  UART0_putchar('\n');		         // newline
  UART0_putchar('\r');		         // carriage return
  
  initPortPins();                           // Initialize port pins
  initLCD_A();                              // Initialize LCD_A
  IE1 &= ~WDTIE;
  _EINT();  
  TACTL = TASSEL_1 + MC_1; // Select smclk/8 and up mode   
  TACCR0 = 32768;               // 1s interval   
  TACCTL0&= ~CCIE;                // Capture/compare interrupt enable 
  

  
  for(;;)
  {              // LPM3, enable interrupts
     int len = 18;
		while(!(rx_flag&0x01));       // wait until receive the character from HyperTerminal
		rx_flag = 0;                  // clear rx_flag
                
		switch (ch) {
                            case  '1' : 
                                            stopflag=0;
                                            for(int i = 0; i < 52; i++) {
                                                      ch = gm2[i];
                                                      UART0_putchar(ch);		      
                                              }
                                              P1IE |= BIT0;               // P1.0 interrupt enabled
                                              P1IES |= BIT0;
					      P1IFG &= ~BIT0;
					break;
			    case  '2'  :

						for(int i = 0; i < 67; i++) {
                                                      ch = gm3[i];
                                                      UART0_putchar(ch);		      
                                              }
                                              for(int i = 0; i < 2; i++){
                                              while(!(rx_flag&0x01));       // wait until receive the character from HyperTerminal
                                                  rx_flag = 0; 
                                                  UART0_putchar(ch);
                                                  mes[len++]=ch;
                                              }
                                              UART0_putchar(':');
                                              mes[len++]=':';
                                              A:
                                              while(!(rx_flag&0x01));       // wait until receive the character from HyperTerminal
                                                  rx_flag = 0;
                                              if(ch>'5')
                                                goto A;
                                                  UART0_putchar(ch);
                                                  mes[len++]=ch;
                                             
                                               while(!(rx_flag&0x01));       // wait until receive the character from HyperTerminal
                                                  rx_flag = 0;
                                                  UART0_putchar(ch);
                                                  mes[len++]=ch;
					      UART0_putchar('\n');		         // newline
                                              UART0_putchar('\r');
                                              // carriage return 
                                              for(int i = 0; i < 31; i++) {
                                                      ch = mes[i];
                                                      UART0_putchar(ch);		      
                                              }
                                              t=(int)mes[18]-48;
                                              t1=(int)mes[19]-48;
                                              t2=(int)mes[21]-48;
                                              t3=(int)mes[22]-48;
                                                P1IE |= BIT1;               // P1.0 interrupt enabled
                                                P1IES |= BIT1;              
                                                P1IFG &= ~BIT1;
                                    break;
                case  '3':
                            for(int i = 0; i < 24; i++) {
                                                                ch = gm4[i];
                                                                UART0_putchar(ch);		      
                                                        }
                            stopflag=1;
                            IE1&= ~WDTIE;
                            
                            break;
                  
                  
}
  }
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR (void)
{

// Chech if Switch 1 is pressed  
   if ((SW1) == 0 && stopflag==0){
     
     for(int i=0; i<=1000; i++);
     if ((SW1) == 0){
        sw1++;
        if(sw1==1)
          IE1|= WDTIE;
        if(sw1==2)
          IE1&= ~WDTIE;
        if(sw1==3){
          resetTime();
          sw1=0;
        }
     }
    }
   if ((SW2) == 0){
     
     for(int i=0; i<=1000; i++);
     if ((SW2) == 0){
       sw2++;
       if(sw2==1){
        TACCTL0|= CCIE;
        setTime(t3,t2,t1,t);
       }
       if(sw2==2){
         TACCTL0&= ~CCIE;
         P3DIR &= ~BIT5;   
         sw2=0;
       }
     }
   }
   P1IFG &= ~BIT0;              // disable interrrupt flags
   P1IFG &= ~BIT1;
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
  static int i=0;
  i++;
    if(i==2){
    testChar();
    testSpecialChar(); 
    i=0;
    }
}


#pragma vector=TIMERA0_VECTOR
__interrupt void TIMERA_ISA (void) 
{   
         decreaseTime();      
	_BIC_SR_IRQ(LPM0_bits); // Clear LPM0 bits from 0(SR) 
}



//
// Initialize port pins
//
void initPortPins(void)
{
  P2DIR = PIN2+PIN1;                        // Set P2.2,1 as outputs
  P5DIR = PIN1;                             // Set P5.1 as output
  P2OUT = PIN1;                             // Set P2.1 to 1
}


// Interrupt for USCI Rx 
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIB0RX_ISR (void)
{
	ch = UCA0RXBUF;		                     // character received is moved to a variable
	rx_flag=0x01;                             // signal main function receiving a char
}
