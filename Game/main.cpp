#include <cstdio>
#include <sys/time.h>
#include <sys/resource.h>

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
	setpriority(PRIO_PROCESS, 0, -20);

	int depth;
	if(argc > 1) {
		depth = atoi(argv[1]);
	} else {
		depth = 5;
	}
	
	Game game = Game();

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