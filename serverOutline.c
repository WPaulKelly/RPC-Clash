#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int GRIDWIDTH = 15;
const int GRIDHEIGHT = 15;

//Code copied from stackOverflow
#include <termios.h>

static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}

typedef struct p {
	int x;
	int y;
	int widthr;
	int heightr;
} player;

int main(){
	char in = 0;
	
	char* grid = malloc(GRIDWIDTH*GRIDHEIGHT*sizeof(char));
	int x, y;
	player player1 = {7, 7, 1, 1};
	
	while(in != '\n'){
		system("@cls||clear");	//Clears console
		
		//Redetermine contents of grid based on player position
		for(y = 0; y < GRIDHEIGHT; ++y){
			for(x = 0; x < GRIDWIDTH; ++x){
				if(player1.x - player1.widthr <= x && player1.x + player1.widthr >= x
					&& player1.y - player1.heightr <= y && player1.y + player1.heightr >= y)
					*(grid + x + y*GRIDWIDTH) = '#';
				else
					*(grid + x + y*GRIDWIDTH) = '_';
			}
		}
		
		//Draw grid to console
		for(y = 0; y < GRIDHEIGHT; ++y){
			for(x = 0; x < GRIDWIDTH; ++x)
				printf("%c", *(grid + x + y*GRIDWIDTH));
			printf("\n");
		}
		
		//Get next input from player
		in = 0;
		while(in == 0){
			in = getch();
			
			switch(in){
				case 'w':
					player1.y = fmax(player1.heightr, player1.y - player1.heightr);
					break;
				case 'a':
					player1.x = fmax(player1.widthr, player1.x - player1.widthr);
					break;
				case 's':
					player1.y = fmin(GRIDHEIGHT - player1.heightr - 1, player1.y + player1.heightr);
					break;
				case 'd':
					player1.x = fmin(GRIDWIDTH - player1.widthr - 1, player1.x + player1.widthr);
					break;
				case 10: break;
				default:
					in = 0;
			}
		}
	}
}