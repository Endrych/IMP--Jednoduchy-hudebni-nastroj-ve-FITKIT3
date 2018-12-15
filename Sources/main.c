/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))

/* Constants specifying delay loop duration */
#define	DELAY_T 200000

/* Mapping of LEDs and buttons to specific port pins: */
// Note: only D9, SW3 and SW5 are used in this sample app
#define LED_D9  0x20 // Port B, bit 5
#define LED_D10 0x10 // Port B, bit 4
#define LED_D11 0x8 // Port B, bit 3
#define LED_D12 0x4 // Port B, bit 2

#define COL1 (1<<27)
#define COL2 (1<<29)
#define COL3 (1<<26)
#define COL4 (1<<28)

/*
 * ROW1 23 PTA8
 * ROW2 24 PTA10
 * ROW3 25 PTA6
 * ROW4 26 PTA11
 * COL1 35 PTA27
 * COL2 36 PTA29
 * COL3 37 PTA26
 * COL4 38 PTA28
*/
unsigned int compare = 0x200;

/* A delay function */
void delay(long long bound) {

  long long i;
  for(i=0;i<bound;i++);
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK;

    /* Set corresponding PTB pins (connected to LED's) for GPIO functionality */
    PORTB->PCR[5] = PORT_PCR_MUX(0x01); // D9
    PORTB->PCR[4] = PORT_PCR_MUX(0x01); // D10
    PORTB->PCR[3] = PORT_PCR_MUX(0x01); // D11
    PORTB->PCR[2] = PORT_PCR_MUX(0x01); // D12

    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    PORTA->PCR[8] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[10] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[6] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    PORTA->PCR[27] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[29] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[26] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[28] = PORT_PCR_MUX(0x01); // SW6

    PTA->PDDR = GPIO_PDDR_PDD(0x3C000000);
    /* Change corresponding PTB port pins as outputs */
    PTB->PDDR = GPIO_PDDR_PDD( 0x3C );
    PTB->PDOR |= GPIO_PDOR_PDO( 0x3C); // turn all LEDs OFF

}

void LPTMR0_IRQHandler(void)
{
    // Set new compare value set by up/down buttons
    LPTMR0_CMR = compare; // !! the CMR reg. may only be changed while TCF == 1
    LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK; // writing 1 to TCF tclear the flag
    GPIOB_PDOR ^= LED_D9; // invert D9 state
}

void LPTMR0Init(int count)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;     // Turn OFF LPTMR to perform setup

    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK  // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(1)) ; // use the choice of clock

    LPTMR0_CMR = count;  // Set compare value

    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK   // Clear any pending interrupt (now)
                 | LPTMR_CSR_TIE_MASK   // LPT interrupt enabled
                );

    NVIC_EnableIRQ(LPTMR0_IRQn);

    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   // Turn ON LPTMR0 and start counting
}

void resetPort()
{
	 PORTA->PCR[8] = PORT_PCR_MUX(0x01); // SW6
	 PORTA->PCR[10] = PORT_PCR_MUX(0x01); // SW6
	 PORTA->PCR[6] = PORT_PCR_MUX(0x01); // SW6
	 PORTA->PCR[11] = PORT_PCR_MUX(0x01); // SW6
}
void setColHigh()
{
	PTA->PDOR = GPIO_PDOR_PDO(0x3C000000);
}

void setCol1()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x1C000000);

}
void setCol2()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x2C000000);

}
void setCol3()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x34000000);

}

void setCol4()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x38000000);

}

int main(void)
{
    MCUInit();
    PortsInit();
    LPTMR0Init(compare);
    delay(100000);
    while (1) {
    	setCol1();
    	resetPort();
    	delay(500);
    	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 29))){
    		compare = 0x40; //2
        }
    	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 29))){
    	    compare = 0x40; //5
    	}
    	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 29))){
    		compare = 0x400;  // 8
    	}
    	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 29))){
    	    compare = 0x400; // 0
    	}

    	setCol2();
    	resetPort();
    	delay(500);

    	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 28))){
    		compare = 0x400; //3
    	}
    	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 28))){
    		compare = 0x40;
    	}
    	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 28))){
    		compare = 0x40;
    	}
    	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 28))){
    		compare = 0x400;
    	}

    	setCol3();
    	resetPort();
    	delay(500);

    	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 27))){
    		compare = 0x400; //3
    	}
    	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 27))){
    		compare = 0x400;
    	}
    	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 27))){
    		compare = 0x40;
    	}
    	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 27))){
    		compare = 0x40;
    	}
    	setCol4();
    	resetPort();
    	delay(500);

    	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 26))){
    		compare = 0x40; //3
    	}
    	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 26))){
    		compare = 0x400;
    	}
    	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 26))){
    		compare = 0x400;
    	}
    	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 26))){
    		compare = 0x40;
    	}

        if (compare < 0x40) compare = 0x40;
        if (compare > 0x400) compare = 0x400;
    }

    return 0;
}
