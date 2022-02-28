#include <cstdio>

#include "Game.hpp"

const int MAXMOVESPOSSIBLE = 220;


int main() {
	Game game("3r2k1/8/8/3Pp3/8/8/8/3K4 w - e6 0 2");
	game.printBoard();
	Move moveStart[MAXMOVESPOSSIBLE];
	Move * moves = moveStart;
	fflush(stdout);
	game.getLegalMoves(&moves);
	fflush(stdout);
	game.printMoveList(moveStart, moves);
	fflush(stdout);
}
