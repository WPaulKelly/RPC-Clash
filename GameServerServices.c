/*
 * William Paul Kelly
 * Independent creation
 * 7/22/2018
 * Proof of concept for online multiplayer game system
 *
 * This code relies on Oracle's Sun rpc to function.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GameServer.h"

bool INITCOMPLETE = false;
#define GRIDWIDTH 15
#define GRIDHEIGHT 15
#define MAXPLAYERS 10
#define UIDSENTINEL 0
#define PIDSENTINEL -1
#define PWIDTHR 1
#define PHEIGHTR 1

typedef struct {
	int x;
	int y;
	int widthr;
	int heightr;
	int state;	//Would be an enum, but those are finicky in c
} player;

//Global variables, bad practice
int curPlayers = 0;
player** playerArray;
int playerIds[MAXPLAYERS] = {0};
char pidtochar[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
char* grid;

int mymax(int a, int b){ return a > b ? a : b; }

int mymin(int a, int b){ return a < b ? a : b; }

void init(){
	playerArray = malloc(MAXPLAYERS*sizeof(player*));
	int i;
	for(i = 0; i < MAXPLAYERS; ++i)
		*(playerArray+i) = malloc(sizeof(player));
	
	grid = malloc(GRIDWIDTH*GRIDHEIGHT*sizeof(char));
	int x, y;
	for(y = 0; y < GRIDHEIGHT; ++y)
		for(x = 0; x < GRIDWIDTH; ++x)
			*(grid + x + y*GRIDWIDTH) = '_';
	
	INITCOMPLETE = true;
}

void updateGrid(){
	if(INITCOMPLETE == false) init();
	
	int x, y, p;
	//Could be optimized to stop after checking curPlayers number of players
	//Could also be optimized to not check for player centers around grid borders
	for(y = 0; y < GRIDHEIGHT; ++y){
		for(x = 0; x < GRIDWIDTH; ++x){
			*(grid + x + y*GRIDWIDTH) = '_'; //Keeps player drawing from taking previous positions into account
			for(p = 0; p < MAXPLAYERS; ++p){
				player* curp = (player*)*(playerArray+p);
				//Does the current index even have a player in it
				if(playerIds[p] == UIDSENTINEL)
					continue;
				//Is the current grid tile the center of a player
				if(curp->x == x && curp->y == y){
					//DEBUG
					printf("Found player %d at coords %d, %d\n", p + 1, x, y);
					*(grid + x + y*GRIDWIDTH) = pidtochar[p];
				}
				//Is the current grid tile a part of a player
				else if(curp->x - curp->widthr <= x && curp->x + curp->widthr >= x
					&& curp->y - curp->heightr <= y && curp->y + curp->heightr >= y){
					//What state is the player currently in
					if(curp->state == 2)
						*(grid + x + y*GRIDWIDTH) = '#';
					else if(curp->state == 1)
						*(grid + x + y*GRIDWIDTH) = '~';
				}
				//If neither, keeps the current tile value
			}
		}
	}
}

int addPlayer(int uid){
	//DEBUG
	printf("Got call to add player for uid %x\n", uid);
	
	if(curPlayers == MAXPLAYERS)
		return PIDSENTINEL;
	
	int i;
	for(i = 0; i < MAXPLAYERS; ++i){
		if(playerIds[i] == UIDSENTINEL){
			playerIds[i] = uid;
			curPlayers++;
			
			//Construct new player
			player* p = *(playerArray+i);
			p->x = PWIDTHR;
			p->y = PHEIGHTR;
			p->widthr = PWIDTHR;
			p->heightr = PHEIGHTR;
			p->state = 1;
			
			return i;
		}
	}
	//This statement should be unreachable, but prevents the compiler from complaining
	return PIDSENTINEL;
}

void removePlayer(int uid){
	int i;
	for(i = 0; i < MAXPLAYERS; ++i)
		if(playerIds[i] == uid){
			playerIds[i] = UIDSENTINEL;
			curPlayers--;
			return;
		}
}

//Could be optimized to stop searching after finding curPlayers valid uids
int uidtopid(int uid){
	int i;
	for(i = 0; i < MAXPLAYERS; ++i)
		if(playerIds[i] == uid)
			return i;
	return PIDSENTINEL;
}

char* sendcommand_1_svc(char* in,  struct svc_req *rqstp){
	if(INITCOMPLETE == false) init();
	
	int uid = ((struct authunix_parms*)rqstp->rq_clntcred)->aup_uid;
	int pid = uidtopid(uid);
	if(pid == PIDSENTINEL)
		pid = addPlayer(uid);	//Check if valid pid was created, send error to client if not
	player* p = (player*)*(playerArray+pid);
	
	//DEBUG
	printf("Got call to sendcommand with current pids:\t");
	for(int i = 0; i < MAXPLAYERS; ++i)
		printf("%x\t", playerIds[i]);
	printf("\n");
		
	static char out;
	int i;
	
	switch(*in){
		case 'w':	//Move up
			//Check spaces above for player borders
			for(i = p->x - p->widthr; i <= p->x + p->widthr; ++i){
				if(*(grid + i + (p->y - p->heightr - 1)*GRIDWIDTH) == '#'){
					i = -1;
					break;
				}
			}
			p->y = (i == -1) ? p->y : mymax(p->heightr, p->y - p->heightr);
			out = 1;
			break;
		case 'a':	//Move left
			//Check spaces to the left for player borders
			for(i = p->y - p->heightr; i <= p->y + p->heightr; ++i){
				if(*(grid + (p->x - p->widthr - 1) + i*GRIDWIDTH) == '#'){
					i = -1;
					break;
				}
			}
			p->x = (i == -1) ? p->x : mymax(p->widthr, p->x - p->widthr);
			out = 2;
			break;
		case 's':	//Move down
			//Check spaces below for player borders
			for(i = p->x - p->widthr; i <= p->x + p->widthr; ++i){
				if(*(grid + i + (p->y + p->heightr + 1)*GRIDWIDTH) == '#'){
					i = -1;
					break;
				}
			}
			p->y = (i == -1) ? p->y : mymin(GRIDHEIGHT - p->heightr - 1, p->y + p->heightr);
			out = 3;
			break;
		case 'd':	//Move right
			//Check spaces to the right for player borders
			for(i = p->y - p->heightr; i <= p->y + p->heightr; ++i){
				if(*(grid + (p->x + p->widthr + 1) + i*GRIDWIDTH) == '#'){
					i = -1;
					break;
				}
			}
			p->x = (i == -1) ? p->x : mymin(GRIDWIDTH - p->widthr - 1, p->x + p->widthr);
			out = 4;
			break;
		case 10:	//Disconnect
			removePlayer(uid);
			out = 5; 
			break;
		case ' ':	//Spawn
			switch(p->state){
				case 0: p->state = 1; break;
				case 1: p->state = 2; break;
				case 2: break;
			}
		default: out = -1;
	}
	
	updateGrid();
	
	return &out;
}

char** getgrid_1_svc(void* vptr, struct svc_req *rqstp){
	if(INITCOMPLETE == false) init();
	static char** outGrid;
	outGrid = (char**)malloc(sizeof(char*));
	*outGrid = (char*)malloc(GRIDWIDTH*GRIDHEIGHT*sizeof(char));
	
	int x, y;
	for(y = 0; y < GRIDHEIGHT; ++y){
		for(x = 0; x < GRIDWIDTH; ++x)
			*(*outGrid + x + y*GRIDWIDTH) = *(grid + x + y*GRIDWIDTH);
	}
	
	return outGrid;
}

int* getxmax_1_svc(void* vptr, struct svc_req *rqstp){ /*DEBUG*/ printf("Got call to getxmax\n"); static int out = GRIDWIDTH; return &out; }

int* getymax_1_svc(void* vptr, struct svc_req *rqstp){ /*DEBUG*/ printf("Got call to getymax\n"); static int out = GRIDHEIGHT; return &out; }