#ifndef UCI_H
#define UCI_H

#include "Game.hpp"

enum EngineCommand {
	CONTINUE,		// Continue doing whatever the engine is doing
	START_SEARCH,	// Start a new search
	HALT,			// Stop the current search
	QUIT_IMMEDIATE	// Quit the program
};

enum EngineState {
	WAITING,
	SEARCHING,
	PONDERING
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

struct EngineGoOptions {
	Move searchMoves[MAXMOVESPOSSIBLE];
	uint8_t ponder;
	int32_t whiteTimeMs;
	int32_t blackTimeMs;
	int32_t whiteIncrementPerMoveMs;
	int32_t blackIncrementPerMoveMs;
	int32_t movesToGo;
	int depth;
	uint64_t nodes;
	int mate;
	int32_t moveTime;
	uint8_t infinite;
	int perft;
};

EngineCommand parseInput(char userInput[2000], Game * game, EngineGoOptions * options);
EngineCommand parseCommandUci(Game * game, EngineGoOptions * options);
EngineCommand parseCommandDebug(Game * game, EngineGoOptions * options);
EngineCommand parseCommandIsready(Game * game, EngineGoOptions * options);
EngineCommand parseCommandSetoption(Game * game, EngineGoOptions * options);
EngineCommand parseCommandRegister(Game * game, EngineGoOptions * options);
EngineCommand parseCommandUcinewgame(Game * game, EngineGoOptions * options);
EngineCommand parseCommandPosition(Game * game, EngineGoOptions * options);
EngineCommand parseCommandGo(Game * game, EngineGoOptions * options);
EngineCommand parseCommandStop(Game * game, EngineGoOptions * options);
EngineCommand parseCommandPonderhit(Game * game, EngineGoOptions * options);
EngineCommand parseCommandQuit(Game * game, EngineGoOptions * options);

void sendID();
void sendOptions();
void uciok();
void readyok();
void toLowerCase(char * input);
Move parseMove(Game * game, char * input);
int parseInt(char * token);

#endif