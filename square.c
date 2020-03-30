/*
SQUARE GB - euclidean percussion sequencer for the Gameboy        
written by                                  
(c) David Haselberger

This code is not optimized. It is for study and experimentational use only. 

WITH SPECIAL THANKS TO:
Oliver Wittchow and Julian Rohrhuber for nanovoice sourcecode:
https://swiki.hfbk-hamburg.de/MusicTechnology/uploads/132/nanovoice.c
AND 
Ian Hattwick, Preston Beebe and Zachary Hale for euclidean sequencer description:
https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/
AND 
BlueMoon968 for rand()-function:
https://github.com/BlueMoon968/gbdk_rand
AND
Brian Norman for performantdelay()-function:
https://github.com/gingemonster/GamingMonstersGameBoySampleCode/blob/master/08_simplejumping/main.c
*/

#include <gb/gb.h>
#include <stdio.h>
#include <time.h>
#include <gb/drawing.h>

#define UIBank 0

#define	MASTER 0
#define SLAVE  1

#define DRUMA 0
#define DRUMB 1
#define DRUMC 2

#define	J_ADOWN 0x18U                      
#define	J_AUP 0x14U
#define	J_ALEFT 0x12U
#define	J_ARIGHT 0x11U

#define	J_BDOWN 0x28U
#define	J_BUP 0x24U
#define	J_BLEFT 0x22U
#define	J_BRIGHT 0x21U

#define	J_SEDOWN 0x48U
#define	J_SEUP 0x44U
#define	J_SELEFT 0x42U
#define	J_SERIGHT 0x41U

#define J_AB 0x30U

unsigned char squareA[] =
{
	0xFF, 0xFF, 0x81, 0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81, 0xFF, 0xFF
};

unsigned char squareB[] =
{
	0x00, 0x00, 0x7E, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E,0x7E, 0x00, 0x00
};


unsigned char squareC[] =
{
	0x00, 0x00,0x00,0x00,0x3C,0x3C,0x3C, 0x3C,0x3C,0x3C,0x3C, 0x3C,0x00,0x00,0x00,0x00
};

unsigned char midiC[] =
{
	0xC3, 0xC3, 0xE7, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF,	0xDB, 0xDB, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3
};

unsigned char offsetC[] =
{
	0x3C, 0x3C, 0x7E, 0x7E, 0xE7, 0xE7, 0xC3, 0xC3,	0xC3, 0xC3, 0xE7, 0xE7, 0x7E, 0x7E, 0x3C, 0x3C
};


unsigned char swingC[] =
{
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x3C, 0x3C,	0x3C, 0x3C, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00
};

unsigned char saveC[] =
{
	0x10, 0x10, 0x18, 0x18, 0x1C, 0x1C, 0x1E, 0x1E,	0x1E, 0x1E, 0x1C, 0x1C, 0x18, 0x18, 0x10, 0x10
};


UINT8 drumA[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UINT8 drumB[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UINT8 drumC[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

UINT8 dXlut[32] = {144,140,136,132,128,124,120,116,112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20};
UINT8 dYlut[32] = {16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140};

UINT8 bucket, i, y, randA, randB, randC, first, last, f, x, xA, xB, xC, delayA, delayB, delayC, g, syncToDrum, clockToSync, numberOfPulsesA, totalStepsA, clkA, numberOfPulsesB, totalStepsB, clkB, numberOfPulsesC, totalStepsC, clkC, offStepA, offStepB, offStepC, cDelay, cOffStep;
static UINT8 current_rand;
unsigned char samp_freq, tempo, watch, bob, todd, pop, modus, padext, joy, pad, seldrum;

void performantdelay(UINT8 numloops){
    UINT8 i;
    for(i = 0; i < numloops; i++){
        wait_vbl_done();
    }     
}

UINT8 rand(void) {
    UINT8 to_add = 123; 
    unsigned bit = 0x7ff;
    current_rand = (current_rand * clock() + to_add) & bit;
    return current_rand;
}

void makeASound()
{
  	NR10_REG = 0x29U;
    NR11_REG = 0x4FU;
    NR12_REG = 0xF1U;
    NR13_REG = 0x6FU;
    NR14_REG = 0xC4U;
    NR51_REG |= 0xFF;
}

void makeBSound()
{
  	NR21_REG = 0x4AU;
    NR22_REG = 0x15U;
    NR23_REG = 0x27U;
    NR24_REG = 0xC7U;
    NR51_REG |= 0xFF;
}

void makeCSound()
{
  	NR41_REG = 0x10U;
    NR42_REG = 0x24U;
    NR43_REG = 0x31U;
    NR44_REG = 0xC0U;
	NR51_REG |= 0xFF;
    //NR52_REG |= 0x80;
}

//Euclidean Drum A starting at first beat
void fillDrumA() {
	bucket = 0;
	for (i = 0; i < totalStepsA; i++) 
	{
    	bucket = bucket + numberOfPulsesA; 
    	if (bucket > (totalStepsA - 1))
		{
			bucket = bucket - totalStepsA;
			drumA[i] = 1;
		}
		else if (bucket < totalStepsA)
		{
			drumA[i] = 0;
		}
    }
    first = drumA[0];
	performantdelay(1);
	//printf("");
  	if ((numberOfPulsesA > 0) && (first != 1)) 
	{ 
  		for (f=0; f < totalStepsA; f++) {
    		for (x=0; x < totalStepsA; x++) {
      		  drumA[x] = drumA[x+1]; 
    	  	}
    		drumA[totalStepsA - 1] = first;
  		  	if (drumA[0] == 1)
			{ 
				
				break; 
			} 
    	}
  	}
	
	last = drumA[totalStepsA - 1];
  	if ((numberOfPulsesA > 0) && (offStepA != 0)) 
	{ 
  		for (f=0; f < offStepA; f++) {
    		for (x=totalStepsA; x > 0; x--) {
      		  drumA[x] = drumA[x-1]; 
    	  	}
    		drumA[0] = last;
    	}
  	}
	randA = rand();
	xA = delayA + randA % 30;
	//printf("(A S:%d P:%d O:%d R:%d)\n", totalStepsA, numberOfPulsesA, offStepA, xA);
}

void fillDrumB() {
	bucket = 0;
	for (i = 0; i < totalStepsB; i++) 
	{
    	bucket = bucket + numberOfPulsesB;
    	if (bucket > (totalStepsB - 1))
		{
			bucket = bucket - totalStepsB;
			drumB[i] = 1;
		}
		else if (bucket < totalStepsB)
		{
			drumB[i] = 0;
		}
    }
    first = drumB[0];
	performantdelay(1);
	//printf("");
  	if ((numberOfPulsesB > 0) && (first != 1)) 
	{ 
  		for (f=0; f < totalStepsB; f++) {
    		for (x=0; x < totalStepsB; x++) {
      		  drumB[x] = drumB[x+1];
    	  	}
    		drumB[totalStepsB - 1] = first; 
  		  	if (drumB[0] == 1) 
			{ 
				
				break; 
			} 
    	}
  	}
	last = drumB[totalStepsB - 1];
  	if ((numberOfPulsesB > 0) && (offStepB != 0)) 
	{ 
  		for (f=0; f < offStepB; f++) {
    		for (x=totalStepsB; x > 0; x--) {
      		  drumB[x] = drumB[x-1]; 
    	  	}
    		drumB[0] = last;
    	}
  	}
	randB = rand();
	xB = delayB + randB % 30;
	//printf("(B S:%d P:%d O:%d R:%d)\n", totalStepsB, numberOfPulsesB, offStepB, xB);
}

void fillDrumC() {
	bucket = 0;
	for (i = 0; i < totalStepsC; i++) 
	{
    	bucket = bucket + numberOfPulsesC; 
    	if (bucket > (totalStepsC - 1))
		{
			bucket = bucket - totalStepsC;
			drumC[i] = 1;
		}
		else if (bucket < totalStepsC)
		{
			drumC[i] = 0;
		}
    }
    first = drumC[0];
	performantdelay(1);
	//printf("");
  	if ((numberOfPulsesC > 0) && (first != 1)) 
	{ 
  		for (f=0; f < totalStepsC; f++) {
    		for (x=0; x < totalStepsC; x++) {
      		  drumC[x] = drumC[x+1]; 
    	  	}
    		drumC[totalStepsC - 1] = first;
  		  	if (drumC[0] == 1)
			{ 
				break; 
			} 
    	}
  	}
	last = drumC[totalStepsC - 1];
  	if ((numberOfPulsesC > 0) && (offStepC != 0)) 
	{ 
  		for (f=0; f < offStepC; f++) {
    		for (x=totalStepsC; x > 0; x--) {
      		  drumC[x] = drumC[x-1]; 
    	  	}
    		drumC[0] = last;
    	}
  	}
	randC = rand();
	xC = delayC + randC % 30;
	
	//printf("(C S:%d P:%d O:%d R:%d)\n", totalStepsC, numberOfPulsesC, offStepC, xC);
}

/*void updateUI()
{	
	puts("");
	//puts(" ");
	puts("\n");
	puts("SQUARE GB \n \n \n \n \n \n \n \n \n \n");
	if(seldrum == 0)
	{
		printf("* ");
	}
	else
	{
		printf("A ");
	}
	printf("S:%d P:%d O:%d R:%d", totalStepsA, numberOfPulsesA, offStepA, xA);
	printf("\n");
	if(seldrum == 1)
	{
		printf("* ");
	}
	else
	{
		printf("B ");
	}
	printf("S:%d P:%d O:%d R:%d", totalStepsB, numberOfPulsesB, offStepB, xB);
	printf("\n");
	if(seldrum == 2)
	{
		printf("* ");
	}
	else
	{
		printf("C ");
	}
	printf("S:%d P:%d O:%d R:%d", totalStepsC, numberOfPulsesC, offStepC, xC);
	printf("\n");
	if(modus != 1)
	{
		printf("Tempo: %d", tempo);
	}
	else
	{
		printf("Tempo: P");
	}
	if(syncToDrum == 1)
	{
		printf(" SYN");
	}
	printf("\n");
	if(modus == 1)
	{
		printf("Modus: Sync");
	}
	else
	{
		printf("Modus: Master");
	}
	printf("\n");
	SHOW_BKG;
	
}*/

void setupDrum()
{
	switch(seldrum)
	{
		case DRUMA:
			fillDrumA();
			break;
		case DRUMB:
			fillDrumB();
			break;
		case DRUMC:
			fillDrumC();
			break;
		default:
			fillDrumA();
			fillDrumB();
			fillDrumC();
	}
	//updateUI();
	
	//syncToDrum = 1;
}

void increaseOffStep()
{
	UINT8 currentOffStep = 0;
	UINT8 currentSteps = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentOffStep = offStepA;
			currentSteps = totalStepsA;
			break;
		case DRUMB:
			currentOffStep = offStepB;
			currentSteps = totalStepsB;
			break;
		case DRUMC:
			currentOffStep = offStepC;
			currentSteps = totalStepsC;
			break;
	}
	currentOffStep += 1;
	if(currentOffStep > (currentSteps - 1))
	{
		currentOffStep = 0;
	}
	switch(seldrum)
	{
		case DRUMA:
			offStepA = currentOffStep;
			break;
		case DRUMB:
			offStepB = currentOffStep;
			break;
		case DRUMC:
			offStepC = currentOffStep;
			break;
	}
    set_sprite_tile(4, 8);
    move_sprite(4, 8, dXlut[currentOffStep]);//8,144
	wait_vbl_done();
	setupDrum();
}

void decreaseOffStep()
{
	UINT8 currentOffStep = 0;
	UINT8 currentSteps = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentOffStep = offStepA;
			currentSteps = totalStepsA;
			break;
		case DRUMB:
			currentOffStep = offStepB;
			currentSteps = totalStepsB;
			break;
		case DRUMC:
			currentOffStep = offStepC;
			currentSteps = totalStepsC;
			break;
	}
	
	if(currentOffStep != 0)
	{
		currentOffStep -= 1;
	}
	switch(seldrum)
	{
		case DRUMA:
			offStepA = currentOffStep;
			break;
		case DRUMB:
			offStepB = currentOffStep;
			break;
		case DRUMC:
			offStepC = currentOffStep;
			break;
	}
    set_sprite_tile(4, 8);
    move_sprite(4, 8, dXlut[currentOffStep]);
	wait_vbl_done();
	setupDrum();
}

void increaseSteps()
{
	UINT8 currentSteps = 1;
	UINT8 currentPulses = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentSteps = totalStepsA;
			currentPulses = numberOfPulsesA;
			set_sprite_tile(0, 0);
			break;
		case DRUMB:
			currentSteps = totalStepsB;
			currentPulses = numberOfPulsesB;
			set_sprite_tile(1, 2);
			break;
		case DRUMC:
			currentSteps = totalStepsC;
			currentPulses = numberOfPulsesC;
			set_sprite_tile(3, 6);
			break;
	}
	currentSteps += 1;
	if(currentSteps > 32)
	{
		currentSteps = 1;
	}
	switch(seldrum)
	{
		case DRUMA:
			totalStepsA = currentSteps;
			break;
		case DRUMB:
			totalStepsB = currentSteps;
			break;
		case DRUMC:
			totalStepsC = currentSteps;
			break;
	}
	move_sprite(seldrum, dYlut[currentPulses-1], dXlut[currentSteps-1]);
	wait_vbl_done();
	setupDrum();
}

void decreaseSteps()
{
	UINT8 currentSteps = 1;
	UINT8 currentPulses = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentSteps = totalStepsA;
			currentPulses = numberOfPulsesA;
			set_sprite_tile(0, 0);
			break;
		case DRUMB:
			currentSteps = totalStepsB;
			currentPulses = numberOfPulsesB;
			set_sprite_tile(1, 2);
			break;
		case DRUMC:
			currentSteps = totalStepsC;
			currentPulses = numberOfPulsesC;
			set_sprite_tile(3, 6);
			break;
	}
	currentSteps -= 1;
	if(currentSteps < 1)
	{
		currentSteps = 32;
	}
	switch(seldrum)
	{
		case DRUMA:
			totalStepsA = currentSteps;
			break;
		case DRUMB:
			totalStepsB = currentSteps;
			break;
		case DRUMC:
			totalStepsC = currentSteps;
			break;
	}
	move_sprite(seldrum, dYlut[currentPulses-1], dXlut[currentSteps-1]);
	wait_vbl_done();
	setupDrum();
}

void increasePulses()
{
	UINT8 currentSteps = 1;
	UINT8 currentPulses = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentSteps = totalStepsA;
			currentPulses = numberOfPulsesA;
			set_sprite_tile(0, 0);
			break;
		case DRUMB:
			currentSteps = totalStepsB;
			currentPulses = numberOfPulsesB;
			set_sprite_tile(1, 2);
			break;
		case DRUMC:
			currentSteps = totalStepsC;
			currentPulses = numberOfPulsesC;
			set_sprite_tile(3, 6);
			break;
	}
	currentPulses += 1;
	if(currentPulses > 32)
	{
		currentPulses = 1;
	}
	switch(seldrum)
	{
		case DRUMA:
			numberOfPulsesA = currentPulses;
			break;
		case DRUMB:
			numberOfPulsesB = currentPulses;
			break;
		case DRUMC:
			numberOfPulsesC = currentPulses;
			break;
	}
	move_sprite(seldrum, dYlut[currentPulses-1], dXlut[currentSteps-1]);
	wait_vbl_done();
	setupDrum();
}

void decreasePulses()
{
	UINT8 currentSteps = 1;
	UINT8 currentPulses = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentSteps = totalStepsA;
			currentPulses = numberOfPulsesA;
			set_sprite_tile(0, 0);
			break;
		case DRUMB:
			currentSteps = totalStepsB;
			currentPulses = numberOfPulsesB;
			set_sprite_tile(1, 2);
			break;
		case DRUMC:
			currentSteps = totalStepsC;
			currentPulses = numberOfPulsesC;
			set_sprite_tile(3, 6);
			break;
	}
	currentPulses -= 1;
	if(currentPulses < 1)
	{
		currentPulses = 32;
	}
	switch(seldrum)
	{
		case DRUMA:
			numberOfPulsesA = currentPulses;
			break;
		case DRUMB:
			numberOfPulsesB = currentPulses;
			break;
		case DRUMC:
			numberOfPulsesC = currentPulses;
			break;
	}
	move_sprite(seldrum, dYlut[currentPulses-1], dXlut[currentSteps-1]);
	wait_vbl_done();
	setupDrum();
}


void increaseSwing()
{
	UINT8 currentDelay = 0;
	switch(seldrum)
	{
		case DRUMA:
			currentDelay = delayA;
			break;
		case DRUMB:
			currentDelay = delayB;
			break;
		case DRUMC:
			currentDelay = delayC;
			break;
	}
	currentDelay += 1;
	switch(seldrum)
	{
		case DRUMA:
			delayA = currentDelay;
			break;
		case DRUMB:
			delayB = currentDelay;
			break;
		case DRUMC:
			delayC = currentDelay;
			break;
	}
    set_sprite_tile(5, 10);
    move_sprite(5, currentDelay+8, 152);
	wait_vbl_done();
	setupDrum();
}

void decreaseSwing()
{
	UINT8 currentDelay = 0;
	switch(seldrum)
	{
		case DRUMA:
			currentDelay = delayA;
			break;
		case DRUMB:
			currentDelay = delayB;
			break;
		case DRUMC:
			currentDelay = delayC;
			break;
	}
	if(currentDelay != 0)
	{
		currentDelay -= 1;
	}
	switch(seldrum)
	{
		case DRUMA:
			delayA = currentDelay;
			break;
		case DRUMB:
			delayB = currentDelay;
			break;
		case DRUMC:
			delayC = currentDelay;
			break;
	}
    set_sprite_tile(5, 10);
    move_sprite(5, currentDelay+8, 152);
	wait_vbl_done();
	setupDrum();
}

void tick()
{
	
	if (clkA > (totalStepsA - 1)) 
	{ 
		clkA = 0; 
	}
	if (clkB > (totalStepsB - 1)) 
	{ 
		clkB = 0; 
	}
	if (clkC > (totalStepsC - 1)) 
	{ 
		clkC = 0; 
	}
	
	if (syncToDrum == 1)
	{
		switch(seldrum)
		{
			case DRUMA:
				clockToSync = clkA;
				break;
			case DRUMB:
				clockToSync = clkB;
				break;
			case DRUMC:
				clockToSync = clkC;
				break;
		}
		//printf("%d", clockToSync);
		if(clockToSync == 0)
		{
			clkA = 0;
			clkB = 0;
			clkC = 0;
			syncToDrum = 0;
			//updateUI();
		}
	}
	
	if(drumA[clkA] == 1)
	{
		if (xA != 0)
			delay(xA);
		makeASound();
	}
	if(drumB[clkB] == 1)
	{
		if (xB != 0)
			delay(xB);
		makeBSound();
	}
	if(drumC[clkC] == 1)
	{
		if (xC != 0)
			delay(xC);
		makeCSound();
	}
	
	clkA++;
	clkB++;
	clkC++;	
	
	
}

void sync()
{
	bob = pop;
	(*(UBYTE *)0xFF02) = 128;
	todd = (*(UBYTE *)0xFF01);
	if (todd == 0 )
		pop = 0;
	else
		pop = 1;
	if (bob != pop)
	{
		tick();
	}
}



void master()
{
	watch++;
	if(watch > tempo)
	{
		watch = 0;
		tick();
	}
}

void player()
{
	if ( modus == MASTER )
	{
		master();
	}
	else
		sync();
}


void tim()
{
	player();
}


void main()
{
    NR50_REG = 0xFF;
    NR51_REG = 0xFF;
    NR52_REG = 0x80;
	watch = 0;
	clkA = 0;
	pop = 0;
	numberOfPulsesA = 1;
	totalStepsA = 16;
	numberOfPulsesB = 2;
	totalStepsB = 16;
	numberOfPulsesC = 4;
	totalStepsC = 16;
	offStepA = 0;
	offStepB = 0;
	offStepC = 0;
	randA = 0;
	randB = 0;
	randC = 0;
	xA = 0;
	xB = 0;
	xC = 0;
	delayA = 1;
	delayB = 1;
	delayC = 1;
	modus = MASTER;
    samp_freq = 164; //16384/164 ~ 100 
    tempo = 20; //100/10 = 10
	seldrum = DRUMA;
	syncToDrum = 0;
	fillDrumA();
	fillDrumB();
	fillDrumC();
	
	SPRITES_8x8;
    set_sprite_data(0, 2, squareA);
    set_sprite_tile(0, 0);
    move_sprite(0, dYlut[numberOfPulsesA], dXlut[totalStepsA]); //16, 144
	
    set_sprite_data(2, 4, squareB);
    set_sprite_tile(1, 2);
    move_sprite(1, dYlut[numberOfPulsesB], dXlut[totalStepsB]); //16, 144
	
    set_sprite_data(4, 6, squareC);
    set_sprite_tile(2, 4);
    move_sprite(2, dYlut[numberOfPulsesC], dXlut[totalStepsC]); //16, 144
	
    set_sprite_data(6, 8, midiC);
    set_sprite_tile(3, 6);
    move_sprite(3, 0, 0);
	
    set_sprite_data(8, 10, offsetC);
    set_sprite_tile(4, 8);
	move_sprite(4, 8, dXlut[offStepA]);
	
    set_sprite_data(10, 12, swingC);
	set_sprite_tile(5, 10);
	move_sprite(5, delayA+8, 152);
 
	
    set_sprite_data(12, 14, saveC);
    set_sprite_tile(6, 12);
    move_sprite(6, 160, 144);
	
	set_sprite_tile(7, 0);
	move_sprite(7,8,16);
	
    disable_interrupts();
    add_TIM(tim);
    enable_interrupts();
    TMA_REG =samp_freq;
  	/* Set clock to 4096 Hertz */
    //TAC_REG = 0x04U;
	//To convert BPM into ms -> divide 60 with BPM and multiply with hundred.
	//To convert ms into BPM -> divide 60 with ms and multiply with 1000.
    TAC_REG = 0x07U;
	//16384Hz
    set_interrupts(VBL_IFLAG | TIM_IFLAG);
	
	DISPLAY_ON;
	SHOW_SPRITES;
	//set_bkg_data(0, 47, alpha);
	//updateUI();
	
    while(!0)
	{
       if ( joypad() != 0 )
           pad++;            
       else
	   {
           pad = 0;
	   }

       if ( joypad() != joy || pad > 10  )
            padext = joypad();
       else
	   {
		   padext = 0;
	   }
           
	   
       if ( joypad() != joy)
	   {
	   		pad = 0;
	   }
            
        joy = joypad();
		
		
        if (joypad() == J_SELECT)
        {
			seldrum += 1;
			if (seldrum == 3)
			{
				seldrum = 0;
			}
			waitpadup();
			cDelay = 0;
			cOffStep = 0;
			switch(seldrum)
			{
				case DRUMA:
					cDelay = delayA;
					cOffStep = offStepA;
					break;
				case DRUMB:
					cDelay = delayB;
					cOffStep = offStepB;
					break;
				case DRUMC:
					cDelay = delayC;
					cOffStep = offStepC;
					break;
			}
			set_sprite_tile(7, seldrum*2);
			move_sprite(7,8,16);
		    set_sprite_tile(5, 10);
		    move_sprite(5, cDelay+8, 152);
		    set_sprite_tile(4, 8);
		    move_sprite(4, 8, dXlut[cOffStep]);//8,144
			wait_vbl_done();
			//updateUI();	
			//printf("Selected %d \n", seldrum);
		}

        if (joypad() == J_START)
        { 
	      	if (modus == 0)
          	{
			   clkA = 0;
			   clkB = 0;
			   clkC = 0;
         	   modus = 1;
		       set_sprite_tile(3, 6);
		       move_sprite(3, 160, 16);
          	}
	      	else
	        {
 			   clkA = 0;
 			   clkB = 0;
 			   clkC = 0;
	           modus = 0;
		       set_sprite_tile(3, 6);
		       move_sprite(3, 0, 0);
			}
			//updateUI();
			//printf("Modus: %d \n", modus);
			wait_vbl_done();
			waitpadup();
        }
		
        if (joypad() == J_DOWN && tempo != 1 )
        { 
        	tempo--;
			//printf("Tempo: %d \n", tempo);
			waitpadup();
			//updateUI();
			
        }

        if (joypad() == J_UP && tempo != 40)
        { 
        	tempo++;
			//printf("Tempo: %d \n", tempo);
			waitpadup();
			//updateUI();
        }
		
		if ( padext == J_BRIGHT)
		{
			increaseSteps();
			performantdelay(5);
		}
		
		if ( padext == J_BLEFT)
		{
			decreaseSteps();
			performantdelay(5);
		}
		
		if ( padext == J_BUP)
		{
			increasePulses();
			performantdelay(5);
		}
		
		if ( padext == J_BDOWN)
		{
			decreasePulses();
			performantdelay(5);
		}
		
		if ( padext == J_AUP)
		{
			increaseOffStep();
			performantdelay(5);
		}
		
		if ( padext == J_ADOWN)
		{
			decreaseOffStep();
			performantdelay(5);
		}
		
		if ( padext == J_ARIGHT)
		{
			increaseSwing();
			performantdelay(5);
		}
		
		if ( padext == J_ALEFT)
		{
			decreaseSwing();
			performantdelay(5);
		}
		
		if( padext == J_AB)
		{
			syncToDrum = 1;
			//printf("Sync On");
			waitpadup();
			//updateUI();
		}		
	}                                               
            
}








