//#define __AVRDEBUG

#undef __AVRDEBUG

#ifdef __AVRDEBUG
#define MIPS		(unsigned long)100
#define DELAYDEC    (unsigned long)10
#define DELAYFULL	(unsigned long)MIPS * 2
#define DELAYTMP	(unsigned long)MIPS / 32
#else
#define MIPS 		(unsigned long)8000
#define DELAYDEC 	(unsigned long)MIPS / 16
#define DELAYFULL	(unsigned long)MIPS * 3
#define DELAYTMP	(unsigned long)MIPS / 32
#endif

#include<avr/io.h>
#include<stdlib.h>

#define KEYUP 		( unsigned char ) 0xFE  
#define KEYDOWN 	( unsigned char ) 0xFD
#define KEYLEFT 	( unsigned char ) 0xFB
#define KEYRIGHT	( unsigned char ) 0xF7
#define DIFUD 1
#define DIFLR 4
#define MAXX 8
#define MAXY 8
#define NOSNAKE -1
#define MINSNAKELEN 4
#define MAXSNAKELEN 8

// possible directions of snake movement
#define 	UP		(unsigned char) KEYUP 
#define 	DOWN 	(unsigned char) KEYDOWN
#define 	RIGHT	(unsigned char) KEYRIGHT
#define		LEFT	(unsigned char) KEYLEFT

unsigned char curDir;
void checkforinput (void);

struct snake
{
	unsigned char indexLedsDisplay;
	unsigned char bitNumber;
};

struct snake SNAKE[MAXSNAKELEN];

// this two-dimesional array holds
// the state of the LEDs in a XxY matrix
unsigned char ledsDisplay [MAXY];

// hold information of snake head and tail
signed char head=NOSNAKE,tail=NOSNAKE;

// counts the length of snake
unsigned char snakeLen=0;

void eatSnake ( char snakesize, char x, char y ,char found );
void addSnake ( char x , char y );
void drawSnake( void );
unsigned char checkLocation( char randX,char randY );
void posStar( signed char* randX, signed char* randY,signed char found );
unsigned char matchStar(signed char randX, signed char randY, signed char x, signed char y);
signed char snakeStrike(signed char headX, signed char headY);
void checkMaxSnakeLen ( unsigned long* refresh, char* headX, char* headY, char* randX, char* randY, signed char* curDir );
void keyPosition( char* key, signed char curDir );


void delay ( unsigned long  d ) 
{
	checkforinput () ;

	for ( int i = 0 ; i < 1000 ; i ++ ) 
		for ( int j = 0 ; j < d ; j ++ ) ;
		
	checkforinput ( );
}

void decodedelay (void)
{
	delay (DELAYTMP);
}

// Delete the tail of Snake if it moves
void eatSnake ( char snakesize, char x, char y ,char found )
{
	if ( snakeLen < snakesize ) return ;
	if ( found )  return;

	//update ledsDisplay
	unsigned char temp=1;
	ledsDisplay[SNAKE[tail].indexLedsDisplay] = (ledsDisplay[SNAKE[tail].indexLedsDisplay])^((temp<<SNAKE[tail].bitNumber));

	if( tail == MAXSNAKELEN-1 )
		tail = 0;
	else
		tail++;

	snakeLen--;
}

void addSnake ( char x , char y )
{
	//unsigned char temp=1;
	if( head == NOSNAKE && tail == NOSNAKE )
	{
	 
		head = 0;
		tail = 0;
	}
	else if ( head == MAXSNAKELEN-1 )
		head = 0;
	else
		head++;

	 // update ledsDisplay
	SNAKE[head].indexLedsDisplay = y-1;
	SNAKE[head].bitNumber = x-1;
	//temp = temp<<(x-1);
	ledsDisplay[y-1] = (ledsDisplay[y-1])^(1<<(x-1));// temp;
	snakeLen++;
}

void drawSnake( void )
{
	for(unsigned char i=0;i<MAXY;i++)
	{
		PORTD	=	i;					
		PORTB	=	ledsDisplay[i];
	}
}
// TODO: change the loop
///////////////////////////////////////////////////////////
unsigned char checkLocation( char randX,char randY )
{
	for(signed char i=tail;i!=head;i++)
	{
		if( (SNAKE[i].indexLedsDisplay==randY-1) && (SNAKE[i].bitNumber==randX-1) )
			return 1;
		if(i==MAXSNAKELEN)
			i=-1;
	}
	return 0;
}
////////////////////////////////////////////////////////////

void posStar( signed char* randX, signed char* randY,signed char found )
{
	if( (found == 1) || (*randX == NOSNAKE && *randY == NOSNAKE) )
	{
		do
		{
		*randX = rand()%MAXX+1; //for always non zero value 1
		*randY = rand()%MAXY+1;
		}
		while( checkLocation(*randX,*randY) );
		//Update the display for star here
		//char temp=1;
		ledsDisplay[*randY-1] = ledsDisplay[*randY-1] ^ (1<<(*randX-1));

	}
}

unsigned char matchStar(signed char randX, signed char randY,signed char x,signed char y)
{
	if( x==randX && y==randY )
	{
		//UPDATE the leds to of star position as it will occupy by snake
		//char temp=1;
		ledsDisplay[randY-1] = ledsDisplay[randY-1] ^ (1<<(randX-1));
		return 1;
	}
	else return 0;
}

// TODO : check 
signed char snakeStrike(signed char headX,signed char headY)
{
	for(signed char i=tail;i!=head;i++)
	{
		

		if( SNAKE[i].indexLedsDisplay == (headY-1) && SNAKE[i].bitNumber == (headX-1) )
		{
			//game over
			//i dont know what to do after this
			return - 1;
		}
		if( i==MAXSNAKELEN )	i=-1;
	}
	return 0;
	
}

void checkMaxSnakeLen ( unsigned long* refresh, char* headX, char* headY, char* randX, char* randY, signed char* curDir )
{
	if( snakeLen == MAXSNAKELEN )
	{
		//restart i.e. reinitialize all variable with increased game speed(level)
		delay(DELAYFULL);
		if( *refresh > DELAYDEC  )
			*refresh -= DELAYDEC;
		*headX = MAXX/2;
		*headY = MAXY/2;
		for(unsigned char i=0;i<MAXY;i++)
			ledsDisplay[i] = 0;

		snakeLen = 0;
		head = NOSNAKE;
		tail = NOSNAKE;
		*randX = NOSNAKE;
		*randY = NOSNAKE;
		*curDir = UP;
	}

}
/*
void keyPosition( char * key, signed char curDir )
{
	*key=0;
	if( curDir==LEFT )
		*key = 1;
	if( curDir==RIGHT )
		*key = 2;
	if( curDir==UP )
		*key = 4;
	if( curDir==DOWN )
		*key = 8;
}
*/

void gameover ( void )
{
	PORTB = 0;

	for ( int i = 0 ; i < 7 ; i++ )
	{	ledsDisplay [i] = 0;
		PORTD = i;
		PORTB = 0;
	}
	
	for ( unsigned char j = 0 ; j < 2 ; j ++ ) 
	{
		// turn the LEDs on
		for ( unsigned char i = 0 ; i < MAXY ; i ++ ) 
		{
			PORTD = i;
			PORTB = 0XFF;
			delay (MIPS/20);
			PORTB = 0x00;
		}
	}
}


//MAIN

int main( void )
{
	DDRA = 0xFF;
	DDRB = 0XFF;
	DDRC = 0x00;
	DDRD = 0xFF;
	
	unsigned long refresh;
	unsigned char minSnakeSize;
	
	signed char headX ,headY , randX , randY ;
	char found;
	
	while ( 1 ) 
	{
		// init global vars
		snakeLen = 0;
		head = NOSNAKE;
		tail = NOSNAKE;

			// move the snake in curDir after 'refresh' seconds
		refresh = MIPS /  8;

		// Store key information bit#1:R, bit#2: L, bit#3: U, bit#4: D
		//char key=0;

		// default snake length
		minSnakeSize = MINSNAKELEN;

		// holds the direction in which snake is moving
		curDir = UP;

		// variables to store the next mouth position of snake
		headX = MAXX/2 + 3;
		headY = MAXY/2 + 3;

		// variables to hold the position of asterik that will be eaten by snake
		// make bitfield if req
		randX=NOSNAKE;
		randY=NOSNAKE;

		// to hold the key presses
		char ch = 0;

		// to determine whether star has found or not
		found = 0; // temporary.. to fix: set to 1!

		for ( signed char i = 0 ; i < MAXY ; i ++ ) 
		{
			ledsDisplay [i] = 0 ;
			PORTD = i;
			PORTB = 0;
		}
		PORTD = 0;

		while ( 1 ) 
		
		{
/*
			if ( PINC != 0 ) ch = PINC;

			// Auto-assume if curdirection is already one which is press then ignore it
			if( ( DIFUD != ( curDir-ch>=0?(curDir-ch):-(curDir-ch) ) ) && DIFLR != (curDir-ch>=0?(curDir-ch):-(curDir-ch)) )
				curDir = ch;
			//Update position of key
			//keyPosition(&key, curDir);
*/

			// determine next mouth position, based on current direction
			if ( curDir == UP )
			{
				headY--;
				if( headY == -1 )
				{  // GAME OVER
					break; 
				}
				
			}
				
			if ( curDir == DOWN )
			{
				headY++;
				if( headY == MAXY+1 )
				{//GAME OVER 
					break ; 
			
				}
			
			}
			
			if ( curDir == LEFT )
			{
				headX--;
				if( headX == -1 )
				{// GAME OVER 
					break ; 
			
				}
			}
			if ( curDir == RIGHT )
			{
				headX++;
				if( headX == MAXX+1 ) 
				{//GAME OVER 
					break; 
			
				}	
			}
			
			// check snake strike with itself
//			if ( snakeStrike(headX,headY) == -1 )  break;
			// determine the head is in position of star or not
//			found = matchStar( randX,randY,headX,headY );
			// position of star
//			posStar(&randX,&randY,found);
			// increase snake size and turn on relevant LED
			addSnake ( headX , headY );
			// eat any end bits of the snake, since we're increasing its size
			eatSnake ( minSnakeSize,headX,headY,found );
			// check for maximum sankeLen
//			checkMaxSnakeLen( &refresh,&headX,&headY,&randX,&randY,&curDir );
			// DrawSnake
			drawSnake();
			
			delay(refresh);
		}
		gameover () ;
	}
	
	return 0;
}

void checkforinput (void)
{
	if ( PINC == 0XFF )
	{	
		curDir = PINC;
		return;
	}
	
	if ( curDir == PINC ) return;

	// we have new input
	curDir = PINC;
	PORTB = 0;
	
	if ( curDir == KEYUP )
		curDir = UP ;
		
	if ( curDir == KEYDOWN )
		curDir = DOWN;
		
	if ( curDir == KEYLEFT ) 
		curDir = LEFT ; 
		
	if ( curDir == KEYRIGHT ) 
		curDir = RIGHT ;

	

}
