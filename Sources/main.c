/**
 *	Jmeno - David Endrych
 *	Login - xendry02
 *	Soubor je editaci souboru z democvika na fitkit3, ale nezustalo v nem temer nic, navic jsem ve zdrojovem souboru pouzil funkci delay z laboratornich cviceni
 *	Podil zmen:  95+% asi podle meho odhadu
 *  Posledni editace 17.12. 2018
 */


/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"


// Makra MASKY COlUMNU
#define COL1 (1<<27)
#define COL2 (1<<29)
#define COL3 (1<<26)
#define COL4 (1<<28)

// Makra pro definici velikosti delaye jednotlivych not
#define NOTE_G 2550
#define NOTE_D 3400
#define NOTE_F 2864
#define NOTE_E 3038
#define NOTE_C 3830
#define NOTE_B 2028
#define NOTE_A 2272
#define NOTE_C1 1912

// Urcuje jestli bylo zmacknute jako posledni tlacitko D (Pouziva se pro prehravani pisnicek)
int wasD = 0;

/*
 * ZAPOJENI Periferii
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

/* Delay funkce pouzita ze cviceni */
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

/*Inicializace portu pro periferni zarizeni*/
void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK;

    PORTA->PCR[8] = PORT_PCR_MUX(0x01);
    PORTA->PCR[7] = PORT_PCR_MUX(0x01);
    PORTA->PCR[10] = PORT_PCR_MUX(0x01);
    PORTA->PCR[6] = PORT_PCR_MUX(0x01);
    PORTA->PCR[11] = PORT_PCR_MUX(0x01);

    PORTA->PCR[27] = PORT_PCR_MUX(0x01);
    PORTA->PCR[29] = PORT_PCR_MUX(0x01);
    PORTA->PCR[26] = PORT_PCR_MUX(0x01);
    PORTA->PCR[28] = PORT_PCR_MUX(0x01);

    PTA->PDDR = GPIO_PDDR_PDD(0x3C000080);
}

/* Zvuz z repredokutoru podle delky delay se urcuje jaky se zahraje ton */
void beep(int lenght) {
int q;
	for (q=0; q<150; q++) {
    	PTA->PDOR = GPIO_PDOR_PDO(0x80);
    	delay(lenght);
    	PTA->PDOR = GPIO_PDOR_PDO(0x0);
    	delay(lenght);
    }
}

/**
 *	Resetovani radku
 */
void resetPort()
{
	 PORTA->PCR[8] = PORT_PCR_MUX(0x01); // SW6
	 PORTA->PCR[10] = PORT_PCR_MUX(0x01); // SW6
	 PORTA->PCR[6] = PORT_PCR_MUX(0x01); // SW6
	 PORTA->PCR[11] = PORT_PCR_MUX(0x01); // SW6
}

/**
 * Nastaveni prvniho sloupce klavesnice
 */
void setCol1()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x1C000000);

}

/**
 * Nastaveni druheho sloupce klavesnice
 */
void setCol2()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x2C000000);

}

/**
 * Nastaveni tretiho sloupce klavesnice
 */
void setCol3()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x34000000);

}

/**
 * Nastaveni ctvrteho sloupce klavesnice
 */
void setCol4()
{
    PTA->PDOR = GPIO_PDOR_PDO(0x38000000);

}

/**
 * Prehrani pisnicky
 * Ocekava seznam not a delay
 */
void playSong(int note[], int noteDelay)
{
	int index = 0;
	while(note[index] != 0){
		beep(note[index]);
		delay(noteDelay);
		index++;
	}
}

/**
 * Spusti prvni nadefinovanou pisnicku pomoci playSong
 */
void playSong1()
{
	int note[] = {NOTE_D,NOTE_G,NOTE_G,NOTE_G,NOTE_D,NOTE_D,NOTE_D,NOTE_B,NOTE_B,NOTE_G,NOTE_G,NOTE_G,NOTE_A,NOTE_A,NOTE_F,NOTE_F,NOTE_G,NOTE_D,
		    		NOTE_G,NOTE_G,NOTE_D,NOTE_D,NOTE_B,NOTE_B,NOTE_G,NOTE_G,NOTE_A,NOTE_A,NOTE_F,NOTE_F,NOTE_G,NOTE_C,NOTE_G,NOTE_F,NOTE_E,NOTE_E,NOTE_C,
		    		NOTE_G,NOTE_F,NOTE_E,NOTE_E,NOTE_E,NOTE_F,NOTE_F,NOTE_B,NOTE_B,NOTE_E,NOTE_A,NOTE_F,NOTE_B,NOTE_A,NOTE_F,NOTE_G
		    		, 0};
	playSong(note,50000);
}

/**
 * Spusti druhou nadefinovanou pisnicku pomoci playSong
 */
void playSong2()
{
	int note[] = {NOTE_F,NOTE_B,NOTE_F,NOTE_B,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_B,NOTE_F,NOTE_B,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_F,
		    		NOTE_C,NOTE_F,NOTE_F,NOTE_C,NOTE_F,NOTE_F,NOTE_C,NOTE_F,NOTE_C,NOTE_F
		    		, 0};

	playSong(note,100000);
}

/**
 * Spusti treti nadefinovanou pisnicku pomoci playSong
 */
void playSong3()
{
	int note[] = {NOTE_G,NOTE_G,NOTE_E,NOTE_G,NOTE_G,NOTE_E, NOTE_G, NOTE_G,NOTE_A,NOTE_G, NOTE_G,NOTE_F,
		    	NOTE_F,NOTE_F,NOTE_D,NOTE_F,NOTE_F,NOTE_D,NOTE_F,NOTE_F,NOTE_G,NOTE_F,NOTE_F,NOTE_E
				, 0};
	playSong(note,100000);

}

/**
 * Spusti ctvrtou nadefinovanou pisnicku pomoci playSong
 */
void playSong4()
{
	int note[] = {NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_G,NOTE_C,NOTE_D,
		    	NOTE_E,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,
		    	NOTE_D,NOTE_D,NOTE_E,NOTE_D,NOTE_G,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,NOTE_E,
		    	NOTE_E,NOTE_G,NOTE_C,NOTE_D,NOTE_E,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_F,NOTE_E,
		    	NOTE_E,NOTE_E,NOTE_E,NOTE_G,NOTE_G,NOTE_F,NOTE_D,NOTE_C
		    	, 0};
	playSong(note,50000);
}


/**
 * Funkce ktera kontroluje jestli byla zmacknuta klavesa a pripadne provede ocekavanou akci
 */
void detectKeyes()
{

	setCol1(); // Aktivace prvniho sloupce
	resetPort(); // Reset radku
	delay(500);

	// Detekce tlacitek sloupce
	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 29))){
		beep(NOTE_G);
		wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 29))){
		beep(NOTE_A);
	    wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 29))){
		beep(NOTE_B);
	    wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 29))){
		beep(NOTE_C1);
	    wasD = 0;
	}

	setCol2(); // Aktivace druheho sloupce
	resetPort(); // Reset radku
	delay(500);

	// Detekce tlacitek sloupce
	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 28))){
		wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 28))){
		wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 28))){
		wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 28))){
		wasD = 1;
	}

	setCol3(); // Aktivace druheho sloupce
	resetPort(); // Reset radku
	delay(500);

	// Detekce tlacitek sloupce
	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 27))){
		beep(NOTE_C);
	    wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 27))){
		beep(NOTE_D);
	    wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 27))){
		beep(NOTE_E);
	    wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 27))){
		beep(NOTE_F);
	    wasD = 0;
	}

	setCol4(); // Aktivace druheho sloupce
	resetPort(); // Reset radku
	delay(500);

	// Detekce tlacitek sloupce
	if(((GPIOA_PDIR & (1<<8)) == 0) && !(PTA->PDOR & (1 << 26))){
		// Prehrani pisnicky p0okud bylo predtim na klavesnici zmackle D
		if(wasD){
			playSong1();
	    }
	    wasD = 0;

	}

	if(((GPIOA_PDIR & (1<<10)) == 0) && !(PTA->PDOR & (1 << 26))){
		// Prehrani pisnicky p0okud bylo predtim na klavesnici zmackle D
		if(wasD){
			playSong2();
	    }
		wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<6)) == 0) && !(PTA->PDOR & (1 << 26))){
		// Prehrani pisnicky p0okud bylo predtim na klavesnici zmackle D
		if(wasD){
			playSong3();
	    }
	    wasD = 0;
	}

	if(((GPIOA_PDIR & (1<<11)) == 0) && !(PTA->PDOR & (1 << 26))){
		// Prehrani pisnicky p0okud bylo predtim na klavesnici zmackle D
		if(wasD){
			playSong4();
	    }
	    wasD = 0;
	}
}

int main(void)
{
    MCUInit();
    PortsInit();
    delay(50000); // Delay aby se na zacatku vse pripravilo

    // Beh programu
    while (1) {
    	detectKeyes();
    }

    return 0;
}
