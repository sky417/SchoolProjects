#include <stdlib.h>
#include <stdio.h>
#include <p89lpc9351.h>

// LCD functions
#include "LCD.c"

// useful macro
#define max(a,b) ((a)<(b)?(b):(a))
#define min(a,b) ((a)>(b)?(b):(a))

#define XTAL 7373000L
#define BAUD 115600L

// pre-defined constants used in the program
#define MAXTURN 100
#define MINTURN 60
#define MINSUM 7
#define MAXSUM 190
#define MAXNORMALSPD 100
#define MAXSIGSPD 100
#define MAXSIGTURN 70
#define SIGNAL 100
#define SIGNAL_1 140
#define SIGNAL_2 110
#define NOSIGNAL 50
#define SCLOCK 50
#define SHARPTURNTIME 50
#define CORNERSIGNAL 330

//We want timer 0 to interrupt every 20 microseconds ((1/50000Hz)=20 us)
#define FREQ 50000L

//The reload value formula comes from the datasheet...
#define TIMER0_RELOAD_VALUE (65536L-((XTAL)/(2*FREQ)))

//These variables are used in the ISR
volatile unsigned char pwmcount;
volatile unsigned char pwm_left=0;
volatile unsigned char pwm_right=0;
volatile unsigned int clockcount;
volatile unsigned char clockStart=0;
volatile unsigned int signalclock = 0;
volatile unsigned char sharpturn=0;
volatile unsigned char sharpturnclock=0;

// ADC initialization
void InitADC(void)
{
	// Set adc1 channel pins as input only
	P0M1 |= (P0M1_4 | P0M1_3 | P0M1_2 | P0M1_1);
	P0M2 &= ~(P0M1_4 | P0M1_3 | P0M1_2 | P0M1_1);
	BURST1 = 1; // Autoscan continuos conversion mode
	ADMODB = CLK0; // ADC1 clock is 7.3728MHz/2
	// Select the four channels for conversion:
	ADINS = (ADI13|ADI12|ADI11|ADI10);
	// Enable the converter and start immediately:
	ADCON1 = (ENADC1|ADCS10);
	while((ADCI1&ADCON1)==0); //Wait for first conversion to complete
}

// Timer 0 initialization
void InitTimer0 (void)
{
	// Initialize timer 0 for ISR 'pwmcounter' below
	TR0=0; // Stop timer 0
	TF0=0; // Clear the overflow flag
	TMOD=(TMOD&0xf0)|0x01; // 16-bit timer
	TH0=TIMER0_RELOAD_VALUE/0x100;
	TL0=TIMER0_RELOAD_VALUE%0x100;
	TR0=1; // Start timer 0
	ET0=1; // Enable timer 0 interrupt
	EA=1;  // Enable global interrupts

	// initialize P1 to Bi-directional
	P1M1=0x00;
	P1M2=0x00;
}

//Interrupt 1 is for timer 0.  This function is executed every 100 us.
void pwmcounter (void) __interrupt 1 __using 1
{
	TF0=0; // Clear the overflow flag
	//Reload the timer
	TR0=0; // Stop timer 0
	TH0=TIMER0_RELOAD_VALUE/0x100;
	TL0=TIMER0_RELOAD_VALUE%0x100;
	TR0=1; // Start timer 0
	if(--pwmcount<1) // starts with negative value
	{
		pwmcount=100; 
		clockcount++;
		if (signalclock>0) signalclock--;
		if (sharpturnclock>0) sharpturnclock--;
	}
	// generate pulse
	P1_6=(pwm_left>pwmcount)?1:0;
	P1_7=(pwm_right>pwmcount)?1:0;
}

// initialize the clock 
void startClock()
{
	clockcount = 0;
	InitLCDclock();
	clockStart = 1;
}

// stop the clock
void stopClock()
{
	clockStart = 0;
}

// stop the rover
void stopRover()
{
	pwm_left = 0;
	pwm_right = 0;
	P1_6 = 0;
	P1_7 = 0;
}
	

// adjust PWM values
// mode 0 : normal driving mode
// mode 1 : (pk_center > SIGNAL) limited turn
// mode 2 : left turn signal received
// mode 3 : right turn signal received
// returns current signal difference
unsigned int drive(unsigned char driveMode, unsigned int pk_left, unsigned int pk_right)
{
	unsigned int signalDiff = 0;
	unsigned int pk_sum = pk_right + pk_left;
	signalDiff = max(pk_right,pk_left) - min(pk_right,pk_left); // take the absolute value

	// amplify the value of the difference
	// as rover goes off the track, the total and signal difference get small
	// but if the rover is going off the track, it has to turn more rapidly
	signalDiff = signalDiff * MAXSUM / (pk_sum); 
	
	if (pk_sum < MINSUM) // out of track, stop the rover
	{
		stopRover();
		return 0;
	}

	if (sharpturnclock == 0) sharpturn = 0; // sharp curve is over

	// force the turn if sharpturn flag is on
	if (sharpturn == 1) goto leftturn; 
	else if (sharpturn == 2) goto rightturn;

	switch (driveMode)
	{
	
	case 0: // normal mode		

		signalDiff = min(signalDiff, MAXTURN); // maximum difference is MAXTURN (100)

		if (pk_left == pk_right) // received same strength signal from both side
		{
			// multiply by (pk_sum / MAXSUM) to prevent going too fast off the track
			pwm_left = MAXNORMALSPD * (pk_sum)/MAXSUM;
			pwm_right = MAXNORMALSPD * (pk_sum)/MAXSUM;
		}
	
		else if (pk_left > pk_right) // stronger signal on left LC - turn right 
		{
			signalDiff = max(signalDiff,MINTURN);
			pwm_left = (MAXNORMALSPD - signalDiff) * (pk_sum)/MAXSUM;
			pwm_right = MAXNORMALSPD * (pk_sum)/MAXSUM;

			if (pk_left < 35 && pk_sum < 70 ) // sharp turn, the opposite side has stronger signal
			{
				sharpturn = 1; // indicates sharp curve mode
				goto leftturn;
			}
			
			else if (pk_left > 100) sharpturn = 0; // turn off sharp turn flag
		}
	
		else if (pk_left < pk_right) // stronger signal on right LC - turn left (similar to the one above)
		{
			signalDiff = max(signalDiff,MINTURN);
			pwm_left = MAXNORMALSPD * (pk_sum)/MAXSUM;
			pwm_right = (MAXNORMALSPD - signalDiff) * (pk_sum)/MAXSUM;

			if (pk_right < 35 && pk_sum < 70 ) 
			{
				sharpturn = 2;
				goto rightturn;
			}
			
			else if (pk_right > 100) sharpturn = 0;
		}
		break;
		
	case 1: // on the signal - do not turn more than MAXSIGTURN
		signalDiff = min(signalDiff, MAXSIGTURN);
		if (pk_left == pk_right) 
		{
			pwm_left = MAXSIGSPD * (pk_sum)/MAXSUM;
			pwm_right = MAXSIGSPD * (pk_sum)/MAXSUM;
			
		}
	
		else if (pk_left > pk_right) // stronger signal on left LC - turn right 
		{
			pwm_left = (MAXSIGSPD - signalDiff) * (pk_sum)/MAXSUM;
			pwm_right = MAXSIGSPD * (pk_sum)/MAXSUM;
		}
	
		else if (pk_left < pk_right) // stronger signal on right LC - turn left
		{
			pwm_left = MAXSIGSPD * (pk_sum)/MAXSUM;
			pwm_right = (MAXSIGSPD - signalDiff) * (pk_sum)/MAXSUM;
		}
		break;
			
	case 2: // left turn mode - stop the left wheel
leftturn:
		pwm_left = 0;
		pwm_right = 100;
		break;
	
	case 3: // right turn mode - stop the right wheel
rightturn:
		pwm_left = 100;
		pwm_right = 0;
		break;
		
	}
	
	return signalDiff;
}


void main (void)
{
	// variable declarations

	// variables for signals
	unsigned int pk_left = 0;
	unsigned int pk_right = 0;
	unsigned int pk_center = 0;
	
	unsigned int signalDiff = 0; // signal difference used in drive() function
	unsigned char signalFlag = 0; // how many signal wires have read
	unsigned char signalRead = 0; // indicates if it is ready to read the next signal wire
	unsigned char driveFlag = 0; // indicates any upcoming turns, start/end
	unsigned char driveMode = 0; 
	unsigned char isTurning = 0;

	// flag to track the start/end
	unsigned char isRunning = 1;
	
	unsigned int i=0;
	
	InitADC(); 
	InitTimer0();	
	InitLCD();
		
	// main loop
	while(1) 
	{
		// read the signals
		pk_left = AD1DAT1;
		pk_right = AD1DAT0;
		pk_center = AD1DAT2;
		
		if (clockcount > 498) 
		{
			if (clockStart) 
				incTime(); // increment time every 50,000 ISRs
			clockcount = 0;
		}

		// isRunning=0, running but the clock hasn't started yet
		// isRunning=1, timed mode
		if (isRunning < 2) 
		{
		
			// possible signal detection
			if (pk_center > SIGNAL) 
			{
				driveMode = 1;
				if (pk_center > SIGNAL_1 && signalRead == 0) // signal detected
				{
					signalFlag++;
					signalRead = 1;
					signalclock = SCLOCK * 5; // next signal has to be read within SCLOCK*10 milliseconds
				}
				
				if (signalFlag > 1 && signalclock == 0 && signalRead == 0) // end of signals - set driveFlag
				{
					driveFlag = signalFlag;
					signalRead = 1;
				}
			}
			
			// gap between signal wires, make it ready again for the next signal
			else if (pk_center < NOSIGNAL && driveFlag == 0) 
			{
				signalRead = 0;
				driveMode = 0;
			}
			
			// only one signal read - not a correct signal read, discard
			else if (signalFlag == 1 && signalclock == 0) signalFlag = 0; 

			else if (signalFlag > 1 && signalclock == 0)  // 
			{
				driveFlag = signalFlag;
				signalFlag = 0;
				signalRead = 1;
				signalclock = SCLOCK*2*5; // looking for the intersection after SCLOCK*20 milliseconds
			}

			else driveMode = 0; // normal mode
			
			// intersections 

			// left turn detected after the signalclock has expired
			if (driveFlag == 2 && signalclock == 0) 
			{
				if (pk_center > SIGNAL) 
				{
					if (signalRead == 1)
					{
						// start turning
						driveMode = 2;
						isTurning = 1;
					}
					
					// keep going until the signal is detected
					if (isTurning == 0)
						driveMode = 1;

					else driveMode = 2;
				}
				
				// keep turning until signal stronger than SIGNAL_2 passes in sequence of right->left 
				if (isTurning > 0) driveMode = 2;
				if (isTurning == 1 && pk_right > SIGNAL_2) isTurning++;
				if (isTurning == 2 && pk_left > SIGNAL_2) isTurning++;
				
				// intersection is over, clear all flags and return to the normal drive mode
				if (isTurning == 3) 
				{
					driveMode = 0;
					driveFlag = 0;
					signalRead = 0;
					signalFlag = 0;
					isTurning = 0;
				}
			}

			// right turn detected after the signalclock has expired, identical to the left turn
			else if (driveFlag == 3 && signalclock == 0)
			{
				if (pk_center > SIGNAL)
				{
					if (signalRead == 1)
					{

						driveMode = 3;
						isTurning = 1;
					}
					
					if (isTurning == 0)
						driveMode = 1;

					else driveMode = 3;
				}
				
				if (isTurning > 0) driveMode = 3;
				if (isTurning == 1 && pk_left > SIGNAL_2) isTurning++;
				if (isTurning == 2 && pk_right > SIGNAL_2) isTurning++;
				
				if (isTurning == 3) 
				{
					driveMode = 0;
					driveFlag = 0;
					signalRead = 0;
					signalFlag = 0;
					isTurning = 0;
				}
			}
			
			else if (driveFlag >= 4) // either start or end signal
			{
				if (isRunning == 0)
				{
					// start signal
					isRunning = 1;
					startClock(); 
					
					driveMode = 0;
					driveFlag = 0;
					signalRead = 0;
					signalFlag = 0;
				}
				
				else if (isRunning == 1)
				{
					// end
					isRunning = 2; // end of track
					stopRover();
					TR0 = 0; // stop the interrupt
				}
			}
			
			signalDiff = drive(driveMode, pk_left, pk_right);		
		}
		
		else stopRover();

		// display some useful information to LCD 
		// every 1000 loops
		
		i++;
		if (i == 1000) {
				
		selectLine(2); // clock uses first line, use second line to display information
		
		// left, center, right signal strength
		write('L');
		write(intTochar(pk_left/100));
		write(intTochar(pk_left/10 - pk_left/100*10));
		write(intTochar(pk_left - pk_left/10*10));		
		write(' ');
		write('C');
		write(intTochar(pk_center/100));
		write(intTochar(pk_center/10 - pk_center/100*10));
		write(intTochar(pk_center - pk_center/10*10));
		write(' ');
		write('R');
		write(intTochar(pk_right/100));
		write(intTochar(pk_right/10 - pk_right/100*10));
		write(intTochar(pk_right - pk_right/10*10));
		
		// running mode
		write(' ');
		write(intTochar(isRunning));
		
		i = 0; 
		
		}
	}
}