#ifndef UCI_H
#define UCI_H

#include "Game.hpp"

enum EngineState {
	CONTINUE,
	HALT,
	QUIT_IMMEDIATE
};

enum Command {
	UCI,
	DEBUG,
	ISREADY,
	SETOPTION,
	REGISTER,
	UCINEWGAME,
	POSITION,
	GO,
    STOP,
    PONDERHIT,
    QUIT,
    UNKNOWN
};

enum EngineOption {
	noOption1,
	noOption2,
	ENGINE_OPTION_END
};

EngineState parseInput(char userInput[2000], Game * game);
EngineState parseCommandUci(Game * game);
EngineState parseCommandDebug(Game * game);
EngineState parseCommandIsready(Game * game);
EngineState parseCommandSetoption(Game * game);
EngineState parseCommandRegister(Game * game);
EngineState parseCommandUcinewgame(Game * game);
EngineState parseCommandPosition(Game * game);
EngineState parseCommandGo(Game * game);
EngineState parseCommandStop(Game * game);
EngineState parseCommandPonderhit(Game * game);
EngineState parseCommandQuit(Game * game);

void sendID();
void sendOptions();
void uciok();
void readyok();
void toLowerCase(char * input);
Move parseMove(Game * game, char * input);

#endif