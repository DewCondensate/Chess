#include <cstdio>

#include "Game.hpp"

const int MAXMOVESPOSSIBLE = 220;

uint8_t convertStringToNum(char * in) {
	return in[0] - 'a' + (8 - (in[1] - '0')) * 8;
}

void move(char * in, char * in2, Flag flag, Game * game) {
	Move move = {convertStringToNum(in), convertStringToNum(in2), flag};
	game->doMove(move);
	game->printBoard();
}

int main(int argc, char * argv[]) {
	int depth;
	if(argc > 1) {
		depth = atoi(argv[1]);
	} else {
		depth = 5;
	}
	// r3k2r/Pppp1ppp/1b3nbN/nPB5/2P1P3/qB3N2/q2P2PP/3Q1RK1 w k - 0 1
	// r3k2r/Pppp1ppp/1b3nbN/nPB5/2P1P3/qB3N2/q2P2PP/3Q1RK1 w qk - 0 1

	// MOST RECENT ERROR:
	// position fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0

	Game game = Game("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
	// move("b4", "c5", NONE, &game);
	// move("b2", "a1", PROMOTEQUEEN, &game);
	// move("a4", "b3", NONE, &game);
	// move("a1", "a2", NONE, &game);
	// move("d1", "d2", NONE, &game);
	// move("a5", "a4", NONE, &game);

	game.printBoard();
	printf("position fen ");
	game.printFen();
	printf("go perft %d\n\n", depth);

	struct timespec start, stop;

	clock_gettime( CLOCK_REALTIME, &start);
	// DO TIMING
	uint64_t count = game.enumeratedPerft(depth);
	clock_gettime( CLOCK_REALTIME, &stop);


	uint64_t accum = ( stop.tv_sec - start.tv_sec ) * 1000000000 + ( stop.tv_nsec - start.tv_nsec );

	printf("Searched %llu nodes in %llu nanoseconds that is %lf nodes per second.\n", count, accum, (double) count * 1000000000 / (double) accum);

}