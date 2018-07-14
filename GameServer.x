#define VERSION 1

%#include <math.h>

program GAME_SERVER{
	version SERVER_VERSION{
		char sendCommand(char) = 1;
		string getGrid() = 2;
		int getXMax() = 3;
		int getYMax() = 4;
	} = VERSION;
} = 88888888;