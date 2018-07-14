#include <stdio.h>
#include "GameServer.h"

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

void displayGrid(char* grid, int xmax, int ymax){
	int x, y;
	
	for(y = 0; y < ymax; ++y){
		for(x = 0; x < xmax; ++x)
			printf("%c", *(grid + x + y*xmax));
		printf("\n");
	}
}

int main(int argc, char** argv){
	//DEBUG
	printf("Game Client started!\n");
	
	if(argc != 2){
		printf("Usage: ./GameClient <Game Server Address>\n");
		exit(1);
	}
	
	//DEBUG
	printf("Creating Client\n");
	
	CLIENT* cli;
	cli = clnt_create(*(argv+1), GAME_SERVER, SERVER_VERSION, "udp");
	if(cli == (CLIENT*)NULL){
		printf("Error in client creation\n");
		return 1;
	}
	
	//DEBUG
	printf("Initializing variables\n");
	char in = 0;
	char result = 0;
	int xmax = *getxmax_1(NULL, cli);
	int ymax = *getymax_1(NULL, cli);
	char* grid = malloc(xmax*ymax*sizeof(char));

	//DEBUG
	printf("Starting loop\n");
	while(in != 10){
		grid = *getgrid_1(NULL, cli);
		
		displayGrid(grid, xmax, ymax);
		
		in = getch();
		result = *sendcommand_1(&in, cli);
		
		//Check that result maps to the input correctly
	}
	
	free(grid);
}