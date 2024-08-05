
//Alex-Cezar Leahu <alex-cezar.leahu@student.tuiasi.ro>
//Mon, May 20, 8:13 PM


#include <msp430.h>

unsigned int i=0;

unsigned int saw[100]={0,20,40,61,81,102,122,143,163,184,204,225,245,266,286,307,
                       327,348,368,389,409,430,450,471,491,512,532,552,573,593,614,
                       634,655,675,696,716,737,757,778,798,819,839,860,880,901,921,
                       942,962,983,1003,1024,1044,1064,1085,1105,1126,1146,1167,1187,
                       1208,1228,1249,1269,1290,1310,1331,1351,1372,1392,1413,1433,1454,
                       1474,1495,1515,1536,1556,1576,1597,1617,1638,1658,1679,1699,1720,
                       1740,1761,1781,1802,1822,1843,1863,1884,1904,1925,1945,1966,1986,
                       2007,2027};

unsigned int tri[100]={0,40,80,120,160,204,244,284,324,368,408,448,488,532,572,612,652,696,
                       736,776,816,860,900,940,980,1024,1064,1104,1144,1184,1228,1268,1308,
                       1348,1392,1432,1472,1512,1556,1596,1636,1676,1720,1760,1800,1840,1884,
                       1924,1964,2004,2048,2004,1964,1924,1884,1840,1800,1760,1720,1676,1636,
                       1596,1556,1512,1472,1432,1392,1348,1308,1268,1228,1184,1144,1104,1064,
                       1024,980,940,900,860,816,776,736,696,652,612,572,532,488,448,408,368,324,
                       284,244,204,160,120,80,40};

unsigned int sin[100]={
                       1000, 1063, 1125, 1187, 1249, 1309, 1368, 1426, 1482, 1536,
                       1588, 1637, 1685, 1729, 1771, 1809, 1844, 1876, 1905, 1930,
                       1951, 1969, 1982, 1992, 1998, 2000, 1998, 1992, 1982, 1969,
                       1951, 1930, 1905, 1876, 1844, 1809, 1771, 1729, 1685, 1637,
                       1588, 1536, 1482, 1426, 1368, 1309, 1249, 1187, 1125, 1063,
                       1000, 937, 875, 813, 751, 691, 632, 574, 518, 464,
                       412, 363, 315, 271, 229, 191, 156, 124, 95, 70,
                       49, 31, 18, 8, 2, 0, 2, 8, 18, 31,
                       49, 70, 95, 124, 156, 191, 229, 271, 315, 363,
                       412, 464, 518, 574, 632, 691, 751, 813, 875, 937};

int offset=1000;
unsigned int signal_selection=1;
int initial_freq=487;


void Init_GPIO()
{
    P1DIR = 0xFF; P2DIR = 0xFF;
    P1REN = 0xFF; P2REN = 0xFF;
    P1OUT = 0x00; P2OUT = 0x00;
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watch dog timer

  Init_GPIO();

  FRCTL0 = FRCTLPW | NWAITS_2;

  __bis_SR_register(SCG0);                           // disable FLL
  CSCTL3 |= SELREF__REFOCLK;                         // Set REFO as FLL reference source
  CSCTL0 = 0;                                        // clear DCO and MOD registers
  CSCTL1 |= DCORSEL_5;                               // Set DCO = 24MHz
  CSCTL2 = FLLD_0 + initial_freq;                             // DCOCLKDIV = 24MHz
  __delay_cycles(3);
  __bic_SR_register(SCG0);                           // enable FLL
  while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));         // FLL locked

  CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;        // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
  CSCTL5 = DIVM_0|DIVS_3; //MCLK=16MHz, SMCLK=2MHz
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watch dog timer

  P1SEL0 |= BIT1;                           // Select P1.1 as OA0O function
  P1SEL1 |= BIT1;                           // OA is used as buffer for DAC

  // P2.3

  P2OUT |= BIT3;                          // Configure P1.3 as pulled-up
  P2REN |= BIT3;                          // P1.3 pull-up register enable
  P2IES |= BIT3;                          // P1.3 Hi/Low edge
  P2IE |= BIT3;


    // P4.1

  P4OUT |= BIT1;                          // Configure P1.3 as pulled-up
  P4REN |= BIT1;                          // P1.3 pull-up register enable
  P4IES |= BIT1;                          // P1.3 Hi/Low edge
  P4IE |= BIT1;                          // P1.3 interrupt enabled

    P1SEL0 |= BIT6 | BIT7;                    // set 2-UART pin as second function
    // P4.3 -> TxD
    //P4.2 ->  RxD
    P4SEL0 = BIT2 | BIT3; // selectam functiile  TxD si RxD

  PM5CTL0 &= ~LOCKLPM5;                     // Disable the GPIO power-on default high-impedance mode
                                              // to activate previously configured port settings

  // Configure reference module
  PMMCTL0_H = PMMPW_H;                      // Unlock the PMM registers
  PMMCTL2 = INTREFEN | REFVSEL_0;           // Enable internal 2V reference
  while(!(PMMCTL2 & REFGENRDY));            // Poll till internal reference settles

  SAC0DAC = DACSREF_1 + DACLSEL_2 + DACIE;  // Select int Vref as DAC reference

  SAC0DAT = 0;                       // Initial DAC data

  SAC0DAC |= DACEN;                         // Enable DAC

  SAC0OA = NMUXEN + PMUXEN + PSEL_1 + NSEL_1;//Select positive and negative pin input
  SAC0OA |= OAPM;                            // Select low speed and low power mode
  SAC0PGA = MSEL_1;                          // Set OA as buffer mode
  SAC0OA |= SACEN + OAEN;                    // Enable SAC and OA

  // Use TB2.1 as DAC hardware trigger
  TB2CCR0 = 20-1;                           // PWM Period/2
  TB2CCTL1 = OUTMOD_6;                       // TBCCR1 toggle/set
  TB2CCR1 = 10;                              // TBCCR1 PWM duty cycle
  TB2CTL = TBSSEL__SMCLK | MC_1 | TBCLR;     // SMCLK, up mode, clear TBR

    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL_1;                    // set ACLK as BRCLK

    // Baud Rate calculation. Referred to UG 17.3.10
    // (1) N=32768/4800=6.827
    // (2) OS16=0, UCBRx=INT(N)=6
    // (4) Fractional portion = 0.827. Refered to UG Table 17-4, UCBRSx=0xEE.
    UCA1BR0 = 6;                              // INT(32768/4800)
    UCA1BR1 = 0x00;
    UCA1MCTLW = 0xEE00;

    UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

  P2IFG &= ~BIT3;
  P4IFG &= ~BIT1;
  __bis_SR_register(GIE);        // Enter LPM3, Enable Interrupt
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = SAC0_SAC2_VECTOR
__interrupt void SAC0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(SAC0_SAC2_VECTOR))) SAC0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(SAC0IV,SACIV_4))
  {
    case SACIV_0: break;
    case SACIV_2: break;
    case SACIV_4:
        i++;
        P2IFG &= ~BIT3;
        if (signal_selection==1){
            SAC0DAT = sin[i]+offset;
        }
        if (signal_selection==2){
            SAC0DAT = saw[i]+offset;
        }
        if (signal_selection==3){
            SAC0DAT = tri[i]+offset;
        }
        if (signal_selection==4){
            SAC0DAT = 1010+offset;
        }
        if(i==99)
            i=0;

        break;
    default: break;
  }
}

// Port 2 interrupt service routine

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;
    if (signal_selection==3){
        signal_selection=1;
    }
    else
    {
        signal_selection++;
    }
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;
    offset+=100;
    if (offset>=2048){
        offset=0;
    }
}

// UART A1
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)

{
  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        if(UCA1RXBUF == '1')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              signal_selection=1;
            }
        if(UCA1RXBUF == '2')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              signal_selection=2;
            }
        if(UCA1RXBUF == '3')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              signal_selection=3;
            }
        if(UCA1RXBUF == '4')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              if (offset<2000)
              {offset+=100;}
            }
        if(UCA1RXBUF == '5')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              offset=1000;
            }
        if(UCA1RXBUF == '6')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              if (offset>0)
                {offset-=100;}
            }
        if(UCA1RXBUF == '7')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              signal_selection=4;
            }
        if(UCA1RXBUF == '8')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              if (initial_freq<617)
              {initial_freq+=10;}
              CSCTL2 = FLLD_0 + initial_freq;
            }
        if(UCA1RXBUF == '9')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              initial_freq=487;
              CSCTL2 = FLLD_0 + initial_freq;
            }
        if(UCA1RXBUF == 'a')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              if (initial_freq>=387)
             {initial_freq-=10;}
              CSCTL2 = FLLD_0 + initial_freq;
            }
        if(UCA1RXBUF == 'x')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              PMMCTL2 = INTREFEN | REFVSEL_0;
            }
        if(UCA1RXBUF == 'y')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              PMMCTL2 = INTREFEN | REFVSEL_1;
            }
        if(UCA1RXBUF == 'z')
            {while(!(UCA1IFG&UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;
              PMMCTL2 = INTREFEN | REFVSEL_2;
            }
    __no_operation();
    break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    default: break;
  }
}