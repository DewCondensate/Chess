#include <cstdio>

#include "Game.hpp"

const int MAXMOVESPOSSIBLE = 220;


int main() {
	Game game = Game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	game.printBoard();
	Move moveStart[MAXMOVESPOSSIBLE];
	Move * moves = moveStart;

	struct timespec start, stop;

	clock_gettime( CLOCK_REALTIME, &start);
	uint64_t count = game.enumeratedPerft(3);
	clock_gettime( CLOCK_REALTIME, &stop);


	uint64_t accum = ( stop.tv_sec - start.tv_sec ) * 1000000000 + ( stop.tv_nsec - start.tv_nsec );

	printf("Executed %llu nodes in %llu nanoseconds that is %lf nodes per second.\n", count, accum, (double) count * 1000000000 / (double) accum);

}