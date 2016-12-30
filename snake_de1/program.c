/* EECE259 Homework 4 Solution
 * Date: April 8, 2011
 *
 * Student1Name: Andrew Kisoo Yoon
 * Student1Number: 10653038
 * Student1UserID: i5l4@ece.ubc.ca
 * Student1Email: sky417@gmail.com
 *
 * Student2Name: Youngik Song
 * Student2Number: 20909032
 * Student2UserID: 
 * Student2Email: yiryans@gmail.com
 */

#include "259macros.h"

#define BLACK	makeColour(0,0,0)
#define GREEN	makeColour(0,63,0)
#define RED		makeColour(63,0,0)
#define BLUE	makeColour(0,0,63)

#define MIN_X	0
#define MIN_Y	0
#define MAX_X	40
#define MAX_Y	60

#define LEFT	0x08
#define RIGHT	0x04
#define PAUSE	0x02

int counter = 0;
int timecounter = 0;
int level = 0;
int time = 0;
char gameON = 0;
char digit[10] = {DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, 
			 DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9 };
short music[] = { 
#include "wavdata2.h" 
};
short effect1[] = {
#include "WNSA_2.h"
};
short effect2[] = {
#include "DEAD.h"
};

int sndrdy;


int velocity_H = 1;
int velocity_V = 0;
	
int snake_xpos[150] = { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10 };
int snake_ypos[150] = { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 };
int snake_length = 10;
char snake_updated = 0;
char snake_extend = 0;
int apple_xpos;
int apple_ypos;

unsigned int rand();

void initGameArea();
void scoreArea(char name_1st[], char name_2nd[], char name_3rd[], 
			int score_1st, int score_2nd, int score_3rd);

void drawStr(int x, int y, char chars[]);
void drawApple();
void drawSnake(int xpos, int ypos);
void remove(int xpos, int ypos);
void drawWall(int xpos, int ypos);
void display7SEG(int score);
void initMessage();
void deleteInitmsg();
void initSnake();
void drawInitSnake(int snake_xpos[], int snake_ypos[]);
void updateSnake();
void moveSnake(char c);
char check();
void gameover(int score, int time);
void deleteGameovermsg();

// Interrupts

/* This ISR will be called every 100ms */
/* Display a counter on the red LEDs */

/* This routine configures device-specific details about interrupts */
void enableCounterIRQ( int interval_cycles, ptr_to_function newISR )
{
	registerISR( IRQ_COUNTER, newISR ); /* specify which ISR to call with COUNTER interrupts */
	*pCOUNTER = -interval_cycles; /* initial counter value */
	*pCOUNTER_RELOAD = -interval_cycles; /* on overflow, start with this value */
	*pCOUNTER_STATUS = 1; /* Device: send interrupts, clear existing interrupt */
	enableInterrupt( IRQ_COUNTER ); /* CPU: allow it to receive COUNTER interrupts */
}

void counterISR()
{
	/* remember: no waiting in an ISR */
	counter++;
	timecounter++;

	if (counter>(2800-level*150) && gameON == 1) 
	{
		updateSnake();
		counter = 0;
	}

	if (timecounter>10000 && gameON == 1)
	{
		time++;
		timecounter = 0;
	}
	/* clear source of COUNTER interrupt before returning */
	*pCOUNTER_STATUS = 1; /* Device: send interrupts, clear existing interrupt */
}


void enableKeyIRQ( int keys_to_watch, ptr_to_function newISR )
{
	registerISR( IRQ_KEY, newISR ); /* specify which ISR to call when KEY interrupts */
	*(pKEY+2) = keys_to_watch; /* allow KEY to send interrupts for KEY3,KEY2 */
	enableInterrupt( IRQ_KEY ); /* allow CPU to receive KEY interrupts */
}

void keyISR()
{
	int key = *pKEY_EDGECAPTURE;
	*pKEY_EDGECAPTURE = 0;

	if (gameON == 0) 
	{
		gameON = 1;
		*pLEDG = 0xFF;
	}

	else if (gameON == 1)
	{
		if (key & PAUSE) *pCOUNTER_STATUS=(*pCOUNTER_STATUS==1)?0:1;
		
		else if (*pCOUNTER_STATUS == 1)
		{
			if((velocity_V == 0) && (key & LEFT))
			{
				velocity_V = -1 * velocity_H;
				velocity_H = 0;
			}
	
			else if((velocity_V == 0) && (key & RIGHT))
			{
				velocity_V = velocity_H;
				velocity_H = 0;
			}

			else if((velocity_H == 0) && (key & LEFT))
			{
				velocity_H = velocity_V;
				velocity_V = 0;
			}
			else if((velocity_H == 0) && (key & RIGHT))
			{
				velocity_H = -1 * velocity_V;
				velocity_V = 0;
			}
		}
	}
}

// Main

int main( int argc, char *argv[] )
{
	// test vars
	char str[4];

	char name_1st[] = "ABC";
	char name_2nd[] = "DEF";
	char name_3rd[] = "GHI";
	int score_1st = 30;
	int score_2nd = 20;
	int score_3rd = 10;

	int keys_to_watch = 0x8 | 0x4 | 0x2;
	long int sndsize_m = sizeof(music);
	long int sndsize_e1 = sizeof(effect1);
	long int sndsize_e2 = sizeof(effect2);
	long int sndpt_m = 0;
	long int sndpt_e1 = 0;
	long int sndpt_e2 = 0;

	char snd_effect = 0;

	int snake_prevxpos;
	int snake_prevypos;
	char snake_status;
	int currentscore = 0;
	
	initInterrupts();
	enableKeyIRQ( keys_to_watch, keyISR );
	enableCounterIRQ( ONE_MS/10, counterISR );

	initScreen();
	fillScreen(BLACK);
	initGameArea();
	scoreArea(name_1st, name_2nd, name_3rd, 
			score_1st, score_2nd, score_3rd);
	display7SEG(currentscore);

	initMessage();
	
	while(1)
	{
		while(gameON==0)
		{
			*pHEX7SEG = digit[gameON];
		}

		currentscore = 0;
		time = 0;
		level = 0;
		snake_status = 0;
		snd_effect = 0;
		
		deleteInitmsg();
		deleteGameovermsg();
		fillScreen(BLACK);
		initGameArea();
		scoreArea(name_1st, name_2nd, name_3rd, 
			score_1st, score_2nd, score_3rd);
		initSnake();
		drawInitSnake(snake_xpos, snake_ypos);
		drawApple();
		display7SEG(currentscore);
		*pCOUNTER_STATUS = 1;

		while(gameON==1)
		{
			
			sndrdy = *pSNDRDY;
			sndrdy = sndrdy>>24;
			while(sndrdy != 0 && snd_effect == 0)
			{
				*pSNDL = music[sndpt_m];
				*pSNDR = music[sndpt_m+1];
				sndpt_m += 2;
				if (sndpt_m >= sndsize_m/2) sndpt_m = 0;
				
				sndrdy = *pSNDRDY;
				sndrdy = sndrdy>>24;
			}

			while(sndrdy != 0 && snd_effect == 1)
			{
				*pSNDL = effect1[sndpt_e1];
				*pSNDR = effect1[sndpt_e1+1];
				sndpt_e1 += 2;
				if (sndpt_e1 >= sndsize_e1/2) 
				{
					snd_effect = 0;
					sndpt_e1 = 0;
				}
				
				sndrdy = *pSNDRDY;
				sndrdy = sndrdy>>24;
			}
			
			if (snake_updated == 1) 
			{
				if (snake_extend > 0) 
				{
					snake_extend--;
					disableInterrupts();
					moveSnake(1);
					enableInterrupts();
				}
				else
				{
					disableInterrupts();
					moveSnake(0);
					enableInterrupts();
				}
		
				snake_status = check();
				if (snake_status == 1) 
				{
					snake_extend += 3;
					drawApple();
					snake_status = 0;
					currentscore++;
					if (level < 10) level++;
					display7SEG(currentscore);
					snd_effect = 1;
					sndpt_e1 = 0;
				}

				else if (snake_status == 2) 
				{
					while(sndpt_e2 < sndsize_e2/2)
					{
						sndrdy = *pSNDRDY;
						sndrdy = sndrdy>>24;
						while(sndrdy != 0)
						{
							*pSNDL = effect2[sndpt_e2];
							*pSNDR = effect2[sndpt_e2+1];
							sndpt_e2 += 2;
							if (sndpt_e2 >= sndsize_e2/2) 
								break;
				
							sndrdy = *pSNDRDY;
							sndrdy = sndrdy>>24;
						}
					}
					sndpt_e2 = 0;
					
					gameover(currentscore, time);
					*pLEDG = 0;
					gameON = 0;
				}
			
			}
		
			snake_prevxpos = snake_xpos[0];
			snake_prevypos = snake_ypos[0];

			drawStr(25, 15, "Time");
			drawChar(30, 15, '0'+time/60/10);
			drawChar(31, 15, '0'+time/60%10);
			drawChar(32, 15, ':');
			drawChar(33, 15, '0'+(time % 60)/10);
			drawChar(34, 15, '0'+(time % 60)%10);

			sprintf(str, "%03d", level);
			drawStr(25, 17, "lvl ");
			drawStr(29, 17, str);

			sprintf(str, "%03d", currentscore);
			drawStr(25, 18, "scr ");
			drawStr(29, 18, str);
		}
	}

	return 0;
}



unsigned int rand()
{
	unsigned int c = *pCOUNTER;
	int hi, lo;
	hi = c >> 16;
	lo = c & 0xffff;
	c = hi ^ lo;
	c = c * 29 + 17;
	return c;
}

void initGameArea()
{
	int i, j;

	// draw game area
	
	for (i=0; i<MAX_Y; i++)
	{
		for (j=0; j<MAX_X; j++)
		{
			if (i == 0 || i == MAX_Y-1)
			{
				drawWall(j*4, i*4);
			}
			else
			{
				if (j == 0 || j == MAX_X-1)
					drawWall(j*4, i*4);
				else
					remove(j*4,i*4);
			}
		}
	}
}

void drawStr(int x, int y, char chars[])
{
	int i;

	for (i=0; i < sizeof(chars); i++)
	{
		drawChar(x+i, y, chars[i]);
	}
}

void scoreArea(char name_1st[], char name_2nd[], char name_3rd[],
			int score_1st, int score_2nd, int score_3rd)
{
	char str[4];

	drawStr(24, 0, "Snak");
	drawStr(28, 0, "e ga");
	drawStr(32, 0, "me");

	drawStr(24, 2, "High");
	drawStr(28, 2, " sco");
	drawStr(32, 2, "res ");

	drawStr(25, 4, "1.");
	drawStr(27, 4, name_1st);
	sprintf(str, "%d", score_1st);
	drawStr(33, 4, str);

	drawStr(25, 5, "2.");
	drawStr(27, 5, name_2nd);
	sprintf(str, "%d", score_2nd);
	drawStr(33, 5, str);

	drawStr(25, 6, "3.");
	drawStr(27, 6, name_3rd);
	sprintf(str, "%d", score_3rd);
	drawStr(33, 6, str);

	drawStr(22, 24, "Cont");
	drawStr(26, 24, "rols");
	drawStr(22, 25, "Butt");
	drawStr(26, 25, "on 3");
	drawStr(30, 25, ": tu");
	drawStr(34, 25, "rn L");
	drawStr(22, 26, "Butt");
	drawStr(26, 26, "on 2");
	drawStr(30, 26, ": tu");
	drawStr(34, 26, "rn R");
	drawStr(22, 27, "Butt");
	drawStr(26, 27, "on 1");
	drawStr(30, 27, ": Pa");
	drawStr(34, 27, "use");


}

void drawApple()
{
	char newapple=0;
	int i;
	int j;

	while(newapple==0)
	{
		apple_xpos = rand() % (MAX_X-2) + 1;
		apple_ypos = rand() % (MAX_Y-2) + 1;

		newapple = 1;
		
		for (i=0; i < snake_length; i++)
		{
			if((apple_xpos == snake_xpos[i]) && (apple_ypos == snake_ypos[i])) 
				newapple = 0;
		}
	}

	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
			drawPixel(apple_xpos*4+i, apple_ypos*4+j, RED);
	}
}

void drawSnake(int xpos, int ypos)
{
	int i;
	int j;
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
			drawPixel(xpos+i, ypos+j, BLUE);
	}
}

void drawWall(int xpos, int ypos)
{
	int i;
	int j;
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
			drawPixel(xpos+i, ypos+j, GREEN);
	}
}


void remove(int xpos, int ypos)
{
	int i;
	int j;
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
			drawPixel(xpos+i, ypos+j, BLACK);
	}
}

void display7SEG(int score)
{
	int sc;
	sc = digit[score/10];
	sc = sc<<8;
	sc = sc + digit[score%10];

	*pHEX7SEG = sc;
}

void initMessage()
{
	int i,j;
	for (i=100; i<210; i++)
	{
		for (j=60; j<150; j++)
			drawPixel(i,j,BLACK);
	}

	drawStr(9, 10, "Pres");
	drawStr(13, 10, "s an");
	drawStr(17, 10, "y ke");
	drawStr(21, 10, "y to");
	drawStr(25, 10, " sta");
	drawStr(29, 10, "rt");

}

void deleteInitmsg()
{
	drawStr(9, 10, "    ");
	drawStr(13, 10, "    ");
	drawStr(17, 10, "    ");
	drawStr(21, 10, "    ");
	drawStr(25, 10, "    ");
	drawStr(29, 10, "  ");
}

void initSnake()
{
	int i;

	for (i=0; i<snake_length; i++)
	{
		snake_xpos[i]=0;
		snake_ypos[i]=0;
	}

	for (i=0; i<10; i++)
	{
		snake_xpos[i] = 19-i;
		snake_ypos[i] = 20;
	}

	velocity_H = 1;
	velocity_V = 0;
	snake_length = 10;
}

void drawInitSnake(int snake_xpos[], int snake_ypos[])
{
	int i;

	for (i=0; snake_xpos[i] != 0; i++)
		drawSnake(snake_xpos[i]*4, snake_ypos[i]*4);
}

void updateSnake()
{
	int i;
	
	for (i=0; i<snake_length; i++)
	{
		snake_xpos[snake_length-i] = snake_xpos[snake_length-1-i];
		snake_ypos[snake_length-i] = snake_ypos[snake_length-1-i];
	}
	if (velocity_H == 0) 
	{
		snake_xpos[0] = snake_xpos[1];
		snake_ypos[0] = snake_ypos[1] + velocity_V;
	}
	else 
	{
		snake_xpos[0] = snake_xpos[1] + velocity_H;
		snake_ypos[0] = snake_ypos[1];
	}
	
	snake_updated = 1;
}

void moveSnake(char c)
{
	int i;

	if (c == 0)
	{	
		remove(snake_xpos[snake_length]*4, snake_ypos[snake_length]*4);
		snake_xpos[snake_length] = 0;
		snake_ypos[snake_length] = 0;
	}
	else if (c == 1) snake_length++;

	drawSnake(snake_xpos[0]*4, snake_ypos[0]*4);
	snake_updated = 0;
}

char check()
{
	char status=0;
	unsigned int i;
	if(snake_xpos[0] == apple_xpos && snake_ypos[0] == apple_ypos)
	{
		status = 1;
	}
	
	if(snake_xpos[0] <= MIN_X || snake_xpos[0] >= MAX_X-1)	status = 2;
	if(snake_ypos[0] <= MIN_Y || snake_ypos[0] >= MAX_Y-1)	status = 2;
	
	for (i=1; i < snake_length; i++)
	{
		if((snake_xpos[0] == snake_xpos[i]) && (snake_ypos[0] == snake_ypos[i]))
			status=2;
	}

	return status;
}

void gameover(int score, int time)
{
	int i, j;
	char str[4];

	*pCOUNTER_STATUS = 0;
	
	for (i=100; i<210; i++)
	{
		for (j=60; j<150; j++)
			drawPixel(i,j,BLACK);
	}

	drawStr(12, 10, "GAME");
	drawStr(17, 10, "OVER");

	drawStr(12, 12, "Scor");
	drawStr(16, 12, "e:");
	sprintf(str, "%03d", score);
	drawStr(19, 12, str);
	
	drawStr(12, 14, "Time");
	drawChar(17, 14, '0'+time/60/10);
	drawChar(18, 14, '0'+time/60%10);
	drawChar(19, 14, ':');
	drawChar(20, 14, '0'+time%60/10);
	drawChar(21, 14, '0'+time%60%10);
}

void deleteGameovermsg()
{
	drawStr(12, 10, "    ");
	drawStr(17, 10, "    ");

	drawStr(12, 12, "    ");
	drawStr(16, 12, "   ");
	drawStr(19, 12, "   ");
	
	drawStr(12, 14, "    ");
	drawStr(17, 14, "    ");
	drawChar(21, 14, ' ');
}
