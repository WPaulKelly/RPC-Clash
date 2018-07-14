#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GameServer.h"

bool INITCOMPLETE = false;
#define GRIDWIDTH 15
#define GRIDHEIGHT 15

typedef struct p {
	int x;
	int y;
	int widthr;
	int heightr;
} player;

player player1 = {7, 7, 1, 1};
char* grid;

int mymax(int a, int b){ return a > b ? a : b; }

int mymin(int a, int b){ return a < b ? a : b; }

void init(){
	//DEBUG
	printf("Running init\n");
	
	grid = malloc(GRIDWIDTH*GRIDHEIGHT*sizeof(char));
	INITCOMPLETE = true;
}

void updateGrid(){
	if(INITCOMPLETE == false) init();
	
	int x, y;
	
	for(y = 0; y < GRIDHEIGHT; ++y){
		for(x = 0; x < GRIDWIDTH; ++x){
			if(player1.x - player1.widthr <= x && player1.x + player1.widthr >= x
				&& player1.y - player1.heightr <= y && player1.y + player1.heightr >= y)
				*(grid + x + y*GRIDWIDTH) = '#';
			else
				*(grid + x + y*GRIDWIDTH) = '_';
		}
	}
}

char* sendcommand_1_svc(char* in,  struct svc_req *rqstp){
	//DEBUG
	printf("Got call to sendcommand\n");
	
	if(INITCOMPLETE == false) init();
	static char out;
	
	switch(*in){
		case 'w':
			player1.y = mymax(player1.heightr, player1.y - player1.heightr);
			out = 1;
			break;
		case 'a':
			player1.x = mymax(player1.widthr, player1.x - player1.widthr);
			out = 2;
			break;
		case 's':
			player1.y = mymin(GRIDHEIGHT - player1.heightr - 1, player1.y + player1.heightr);
			out = 3;
			break;
		case 'd':
			player1.x = mymin(GRIDWIDTH - player1.widthr - 1, player1.x + player1.widthr);
			out = 4;
			break;
		case 10: out = 5; break;
		default: out = -1;
	}
	
	updateGrid();
	
	return &out;
}

char** getgrid_1_svc(void* vptr, struct svc_req *rqstp){
	//DEBUG
	printf("Got call to getgrid\n");
	
	if(INITCOMPLETE == false) init();
	static char** outGrid;
	outGrid = (char**)malloc(sizeof(char*));
	*outGrid = (char*)malloc(GRIDWIDTH*GRIDHEIGHT*sizeof(char));
	
	int x, y;
	for(y = 0; y < GRIDHEIGHT; ++y){
		for(x = 0; x < GRIDWIDTH; ++x)
			**(outGrid + x + y*GRIDWIDTH) = *(grid + x + y*GRIDWIDTH);
	}
	
	return outGrid;
}

int* getxmax_1_svc(void* vptr, struct svc_req *rqstp){ /*DEBUG*/ printf("Got call to getxmax\n"); static int out = GRIDWIDTH; return &out; }

int* getymax_1_svc(void* vptr, struct svc_req *rqstp){ /*DEBUG*/ printf("Got call to getymax\n"); static int out = GRIDHEIGHT; return &out; }