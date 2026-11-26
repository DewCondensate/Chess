#include <cstdio>
#include <sys/time.h>
#include <sys/resource.h>

#include "Game.hpp"
#include "evaluation.hpp"
#include "heuristic.hpp"

#include "uci.hpp"

uint8_t convertStringToNum(char * in) {
	return in[0] - 'a' + (8 - (in[1] - '0')) * 8;
}

char pieceNames[][10] = {"EMPTY", "PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING"};

Move getComputerMove(Game * game, int depth) {
    Move moveStart[MAXMOVESPOSSIBLE];
    Move * moves = moveStart;
    game->getLegalMoves(&moves);

    Move bestMove = iterativeDeepening(game, depth, heuristic);

    return bestMove;
}

Move getPlayerMove(Game * game) {
	Move * output = 0;
	Move moveStart[MAXMOVESPOSSIBLE];
    Move * moves = moveStart;
	game->getLegalMoves(&moves);

    while(!output) {

	    printf("Enter a move: ");
	    char col;
		int row;
		char col2;
		int row2;
		scanf(" %c%d%c%d", &col, &row, &col2, &row2);
		row -= 1;
		row = 7 - row;
		col = col >= 'a' ? col - 'a' + 'A' : col;
		col -= 'A';
		int from = row * 8 + col;

		row2 -= 1;
		row2 = 7 - row2;
		col2 = col2 >= 'a' ? col2 - 'a' + 'A' : col2;
		col2 -= 'A';
		int to = row2 * 8 + col2;
		for(Move * i = moveStart; i < moves; i++) {
			if(i->from == from && i->to == to) {
				output = i;
				break;
			}
		}
	}

	return *output;
}



void move(char * in, char * in2, Flag flag, Game * game) {
	Move move = {convertStringToNum(in), convertStringToNum(in2), flag};
	game->doMove(move);
	game->printBoard();
}

int main() {
	setpriority(PRIO_PROCESS, 0, -20);

	char userInput[2000];

	Game game;
	EngineGoOptions options;


	while(1) {
		fgets(userInput, sizeof(userInput), stdin);
		userInput[strcspn(userInput, "\n")] = 0;
		parseInput(userInput, &game, &options);

		
	}

	/*
	int depth = 7;
	
	Game game = Game((char *) "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	game.printBoard();
	printf("position fen ");
	game.printFen();
	printf("go perft %d\n\n", depth);

	// struct timespec start, stop;
	// uint64_t count;
	// clock_gettime(CLOCK_REALTIME, &start);
	// count = game.enumeratedPerft(depth);
	// clock_gettime(CLOCK_REALTIME, &stop);


	// uint64_t accum = ( stop.tv_sec - start.tv_sec ) * 1000000000 + ( stop.tv_nsec - start.tv_nsec );

	// printf("Searched %llu nodes in:\n%llu nanoseconds\n%llu microseconds\n%llu milliseconds\n%llu seconds\nThat is %0.0lf nodes per second or %0.3lfMN/s.\n", count, accum, accum/1000, accum/1000000, accum/1000000000, (double) count * 1000000000 / (double) accum, (double) count * 1000000000 / (double) accum / 1000000);

	Move moveStart[MAXMOVESPOSSIBLE];
    Move * moves = moveStart;
    game.getLegalMoves(&moves);

    printf("{\n");
    for(int i = 0; i < 6; i++) {
    	printf("\t{\n");
    	for(int j = 0; j < 2; j++) {
    		printf("\t\t{\n");
    		for(int k = 0; k < 8; k++) {
    			printf("\t\t\t");
    			for(int l = 0; l < 8; l++) {
    				printf("%d, ", pieceValues[i][j][k * 8 + l]);
    			}
    			printf("\n");
    		}
    		printf("\t\t}\n");
    	}
    	printf("\t}\n");
    }
    printf("}\n");

	Move move;
	bool playerMove = false;
	int searchDepth = 7;
	while(moves != (Move *) moveStart) {
		printf("Printing Board...\n"); fflush(stdout);
		game.printBoard();
		printf("Getting player turn...\n"); fflush(stdout);
		if(game.getGameState().turn == playerMove) {
			// move = getComputerMove(&game, searchDepth);
			// move = getComputerMove(&game, searchDepth);
			move = getPlayerMove(&game);
		} else {
			move = getComputerMove(&game, searchDepth);
		}
		printf("Playing: %c%d%c%d\n", (move.from % 8) + 'a', 8 - move.from / 8, (move.to % 8) + 'a', 8 - move.to / 8);
		printf("Playing move...\n"); fflush(stdout);
		game.doMove(move);
		printf("Printing FEN...\n"); fflush(stdout);
		game.printFen();
		printf("Getting legal moves...\n"); fflush(stdout);
		moves = moveStart;
    	game.getLegalMoves(&moves);
	}

	game.printBoard();

	if(game.gameResult() == 0) {
		printf("It's a draw!\n");
	} else {
		if(game.getGameState().turn) {
			printf("White wins!\n");
		} else {
			printf("Black wins!\n");
		}
	}

	*/
}