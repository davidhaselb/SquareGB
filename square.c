/*
SQUARE GB - euclidean percussion sequencer for the Game Boy        
written by                                  
(c) David Haselberger

WITH SPECIAL THANKS TO:
Oliver Wittchow and Julian Rohrhuber for nanovoice sourcecode:
https://swiki.hfbk-hamburg.de/MusicTechnology/uploads/132/nanovoice.c
AND 
Ian Hattwick, Preston Beebe and Zachary Hale for euclidean sequencer description:
https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/
AND 
BlueMoon968 for rand()-function
https://github.com/BlueMoon968/gbdk_rand
*/

#include <gb/gb.h>
#include <stdio.h>
#include <time.h>
#include <gb/drawing.h>


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

static UINT8 current_rand;


unsigned char samp_freq, tempo, watch, bob, todd, pop, modus, paddel, joy, pad, seldrum;

UINT8 bucket, i, y, randA, randB, randC, first, last, f, x, xA, xB, xC, delayA, delayB, delayC, g, syncToDrum, clockToSync, numberOfPulsesA, totalStepsA, clkA, numberOfPulsesB, totalStepsB, clkB, numberOfPulsesC, totalStepsC, clkC, offStepA, offStepB, offStepC;
UINT8 drumA[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UINT8 drumB[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UINT8 drumC[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

fixed seed;

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
	printf("");
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
	printf("");
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
	printf("");
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

void updateUI()
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
	
}

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
	updateUI();
	
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
	setupDrum();
}

void increaseSteps()
{
	UINT8 currentSteps = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentSteps = totalStepsA;
			break;
		case DRUMB:
			currentSteps = totalStepsB;
			break;
		case DRUMC:
			currentSteps = totalStepsC;
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
	setupDrum();
}

void decreaseSteps()
{
	UINT8 currentSteps = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentSteps = totalStepsA;
			break;
		case DRUMB:
			currentSteps = totalStepsB;
			break;
		case DRUMC:
			currentSteps = totalStepsC;
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
	setupDrum();
}

void increasePulses()
{
	UINT8 currentPulses = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentPulses = numberOfPulsesA;
			break;
		case DRUMB:
			currentPulses = numberOfPulsesB;
			break;
		case DRUMC:
			currentPulses = numberOfPulsesC;
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
	setupDrum();
}

void decreasePulses()
{
	UINT8 currentPulses = 1;
	switch(seldrum)
	{
		case DRUMA:
			currentPulses = numberOfPulsesA;
			break;
		case DRUMB:
			currentPulses = numberOfPulsesB;
			break;
		case DRUMC:
			currentPulses = numberOfPulsesC;
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
			updateUI();
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

void slave()
{
	bob = pop;
	(*(UBYTE *)0xFF02) = 128;
	todd = (*(UBYTE *)0xFF01);
	if (todd == 0 )
		pop = 0;
	else
		pop = 1;
	if (pop == 1)//bob != pop
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
		slave();
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
	//set_bkg_data(0, 47, alpha);
	updateUI();
	
    while(!0)
	{
       if ( joypad() != 0 )
           pad++;            
       else
	   {
           pad = 0;
	   }

       if ( joypad() != joy || pad > 10  )
            paddel = joypad();
       else
	   {
		   paddel = 0;
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
			updateUI();	
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
          	}
	      	else
	        {
 			   clkA = 0;
 			   clkB = 0;
 			   clkC = 0;
	           modus = 0;
			}
			updateUI();
			//printf("Modus: %d \n", modus);
			waitpadup();
        }
		
        if (joypad() == J_DOWN && tempo != 1 )
        { 
        	tempo--;
			//printf("Tempo: %d \n", tempo);
			waitpadup();
			updateUI();
			
        }

        if (joypad() == J_UP && tempo != 40)
        { 
        	tempo++;
			//printf("Tempo: %d \n", tempo);
			waitpadup();
			updateUI();
        }
		
		if ( paddel == J_BRIGHT)
		{
			increaseSteps();
			delay(200);
		}
		
		if ( paddel == J_BLEFT)
		{
			decreaseSteps();
			delay(200);
		}
		
		if ( paddel == J_BUP)
		{
			increasePulses();
			delay(200);
		}
		
		if ( paddel == J_BDOWN)
		{
			decreasePulses();
			delay(200);
		}
		
		if ( paddel == J_AUP)
		{
			increaseOffStep();
			delay(200);
		}
		
		if ( paddel == J_ADOWN)
		{
			decreaseOffStep();
			delay(200);
		}
		
		if ( paddel == J_ARIGHT)
		{
			increaseSwing();
			delay(50);
		}
		
		if ( paddel == J_ALEFT)
		{
			decreaseSwing();
			delay(50);
		}
		
		if( paddel == J_AB)
		{
			syncToDrum = 1;
			//printf("Sync On");
			waitpadup();
			updateUI();
		}
		
		wait_vbl_done();
	}                                               
            
}








