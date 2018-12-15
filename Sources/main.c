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

#define NOTE_G 2550
#define NOTE_HALF_G 2411
#define NOTE_DOWN_E 3219
#define NOTE_DOWN_B 2150
#define NOTE_D 3400
#define NOTE_DOWN_G 2707
#define NOTE_FS 2707
#define NOTE_F 2864
#define NOTE_E 3038
#define NOTE_DS 3219
#define NOTE_GS 2411
#define NOTE_CS 3615
#define NOTE_C 3830
#define NOTE_B 2028
#define NOTE_A 2272

/*
 * ROW1 23 PTA8
 * ROW2 24 PTA10
 * ROW3 25 PTA6
 * ROW4 26 PTA11
 * COL1 35 PTA27
 * COL2 36 PTA29
 * COL3 37 PTA26
 * COL4 38 PTA28
 * SP 27 PTA7
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
    PORTA->PCR[7] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[10] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[6] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    PORTA->PCR[27] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[29] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[26] = PORT_PCR_MUX(0x01); // SW6
    PORTA->PCR[28] = PORT_PCR_MUX(0x01); // SW6

    PTA->PDDR = GPIO_PDDR_PDD(0x3C000080);
    /* Change corresponding PTB port pins as outputs */
    PTB->PDDR = GPIO_PDDR_PDD( 0x3C );
    PTB->PDOR |= GPIO_PDOR_PDO( 0x3C); // turn all LEDs OFF

}

void beep(int lenght) {
int q;
	for (q=0; q<150; q++) {
    	PTA->PDOR = GPIO_PDOR_PDO(0x80);
    	delay(lenght);
    	PTA->PDOR = GPIO_PDOR_PDO(0x0);
    	delay(lenght);
    }
}

void playC(){
	beep(3830);
}

void playD(){
	beep(3400);
}

void playE(){
	beep(3038);
}

void playF(){
	beep(2864);
}

void playG(){
	beep(2550);
}
void playA(){
	beep(2272);
}

void playH(){
	beep(2028);
}
void playC1(){
	beep(1912);
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
    		playG();
        }
    	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 29))){
    	    compare = 0x40; //5
    	    playA();
    	}
    	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 29))){
    		compare = 0x400;  // 8
    		playH();
    	}
    	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 29))){
    	    compare = 0x400; // 0
    	    playC1();
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
    		playC();
    	}
    	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 27))){
    		compare = 0x400;
    		playD();
    	}
    	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 27))){
    		compare = 0x40;
    		playE();
    	}
    	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 27))){
    		compare = 0x40;
    		playF();
    	}
    	setCol4();
    	resetPort();
    	delay(500);

    	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 26))){
    		int note[] = {NOTE_D,NOTE_G,NOTE_G,NOTE_G,NOTE_D,NOTE_D,NOTE_D,NOTE_B,NOTE_B,NOTE_G,NOTE_G,NOTE_G,NOTE_A,NOTE_A,NOTE_F,NOTE_F,NOTE_G,NOTE_D,
    		    				NOTE_G,NOTE_G,NOTE_D,NOTE_D,NOTE_B,NOTE_B,NOTE_G,NOTE_G,NOTE_A,NOTE_A,NOTE_F,NOTE_F,NOTE_G,NOTE_C,NOTE_G,NOTE_F,NOTE_E,NOTE_E,NOTE_C,
    							NOTE_G,NOTE_F,NOTE_E,NOTE_E,NOTE_E,NOTE_F,NOTE_F,NOTE_B,NOTE_B,NOTE_E,NOTE_A,NOTE_F,NOTE_B,NOTE_A,NOTE_F,NOTE_G

    		    		    		    							, 0};
    		    		    		    	    		int index = 0;
    		    		    		    	    		while(note[index] != 0){
    		    		    		    	    			beep(note[index]);
    		    		    		    	    			delay(50000);
    		    		    		    	    			index++;
    		    		    		    	    		}1
    	}
    	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 26))){
    		compare = 0x400;
    		int note[] = {NOTE_F,NOTE_B,NOTE_F,NOTE_B,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_B,NOTE_F,NOTE_B,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_F,
    				NOTE_C,NOTE_F,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_C,NOTE_F,NOTE_C,NOTE_F

						, 0};
    		int index = 0;
    		while(note[index] != 0){
    			beep(note[index]);
    			delay(100000);
    			index++;
    		}
    	}
    	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 26))){
    		compare = 0x400;
    		int note[] = {NOTE_G,NOTE_G,NOTE_E,NOTE_G,NOTE_G,NOTE_E, NOTE_G, NOTE_G,NOTE_A,NOTE_G, NOTE_G,NOTE_F,
    				NOTE_F,NOTE_F,NOTE_D,NOTE_F,NOTE_F,NOTE_D,NOTE_F,NOTE_F,NOTE_G,NOTE_F,NOTE_F,NOTE_E

    							, 0};
    	    		int index = 0;
    	    		while(note[index] != 0){
    	    			beep(note[index]);
    	    			delay(100000);
    	    			index++;
    	    		}

    	}
    	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 26))){
    		compare = 0x40;
    		int note[] = {NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_G,NOTE_C,NOTE_D,
    				NOTE_E,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,
					NOTE_D,NOTE_D,NOTE_E,NOTE_D,NOTE_G,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,
					NOTE_E,NOTE_G,NOTE_C,NOTE_D,NOTE_E,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_E,
					NOTE_E,NOTE_E,NOTE_E,NOTE_G,NOTE_G,NOTE_F,NOTE_D,NOTE_C

    		    							, 0};
    		    	    		int index = 0;
    		    	    		while(note[index] != 0){
    		    	    			beep(note[index]);
    		    	    			delay(50000);
    		    	    			index++;
    		    	    		}
    	}

        if (compare < 0x40) compare = 0x40;
        if (compare > 0x400) compare = 0x400;
    }

    return 0;
}
