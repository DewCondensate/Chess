#include <cstdio>
#include <sys/time.h>
#include <sys/resource.h>

#include "Game.hpp"

const int MAXMOVESPOSSIBLE = 220;

uint8_t convertStringToNum(char * in) {
	return in[0] - 'a' + (8 - (in[1] - '0')) * 8;
}

int heuristic(GameState * state) {
	return 1;
}

Move getComputerMove(Game * game, int depth, bool maximizingPlayer) {
	int8_t moveList[MAXMOVESPOSSIBLE];
    int8_t * last = moveList;

    Move moveStart[MAXMOVESPOSSIBLE];
    Move * moves = moveStart;

    game->getLegalMoves(&moves);

    int value = INT_MIN;
    int tempVal;
    Move bestMove;
    GameState undoMove = game->getGameState();
	for(Move * i = moveStart; i < moves; i++) {
        game->doMove(*i);
        // Check if game over
        // if(doMove(game, moveList[i])) {
        //     game->setGameState(undoMove);
        //     if(getWinner(game) == maximizingPlayer) {
        //     	return moveList[i];
        //     }

        // }
        tempVal = game->alphaBeta(game, depth, INT_MIN, INT_MAX, !maximizingPlayer, heuristic);
      	if(tempVal > value) {
      		value = tempVal;
      		bestMove = *i;
      	}
        game->setGameState(undoMove);
    }
    return bestMove;
}



void move(char * in, char * in2, Flag flag, Game * game) {
	Move move = {convertStringToNum(in), convertStringToNum(in2), flag};
	game->doMove(move);
	game->printBoard();
}

int main(int argc, char * argv[]) {
	setpriority(PRIO_PROCESS, 0, -20);

	int depth;
	if(argc > 1) {
		depth = atoi(argv[1]);
	} else {
		depth = 5;
	}
	
	Game game = Game((char *) "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

	game.printBoard();
	printf("position fen ");
	game.printFen();
	printf("go perft %d\n\n", depth);

	struct timespec start, stop;
	uint64_t count;
	clock_gettime(CLOCK_REALTIME, &start);
	count = game.enumeratedPerft(depth);
	clock_gettime(CLOCK_REALTIME, &stop);


	uint64_t accum = ( stop.tv_sec - start.tv_sec ) * 1000000000 + ( stop.tv_nsec - start.tv_nsec );

	printf("Searched %llu nodes in:\n%llu nanoseconds\n%llu microseconds\n%llu milliseconds\n%llu seconds\nThat is %0.0lf nodes per second or %0.3lfMN/s.\n", count, accum, accum/1000, accum/1000000, accum/1000000000, (double) count * 1000000000 / (double) accum, (double) count * 1000000000 / (double) accum / 1000000);

}