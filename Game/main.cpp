#include <cstdio>
#include <sys/time.h>
#include <sys/resource.h>

#include "Game.hpp"
#include "evaluation.hpp"

uint8_t convertStringToNum(char * in) {
	return in[0] - 'a' + (8 - (in[1] - '0')) * 8;
}

int pieceValues[][2][64] = {
	// Pawn
	{
		{0,   0,   0,   0,   0,   0,   0,   0, 
		 300, 300, 300, 300, 300, 300, 300, 300, 
		 150, 150, 175, 175, 175, 175, 150, 150, 
		 100, 125, 150, 150, 150, 150, 125, 100, 
		 50,  60,  80,  100, 100, 80,  60,  50, 
		 25,  80,  80,  100, 100, 80,  80,  25, 
		 80,  80,  80,  0,   0,   80,  80,  80, 
		 0,   0,   0,   0,   0,   0,   0,   0},

		{0,   0,   0,   0,   0,   0,   0,   0, 
		 80,  80,  80,  0,   0,   80,  80,  80, 
		 25,  80,  80,  100, 100, 80,  80,  25, 
		 50,  60,  80,  100, 100, 80,  60,  50, 
		 100, 125, 150, 150, 150, 150, 125, 100, 
		 150, 150, 175, 175, 175, 175, 150, 150, 
		 300, 300, 300, 300, 300, 300, 300, 300, 
		 0,   0,   0,   0,   0,   0,   0,   0}},

	// Knight
	{
		{50,  120, 175, 225, 225, 175, 120,  50, 
		 120, 200, 250, 250, 250, 250, 200, 120, 
		 175, 250, 300, 350, 350, 300, 250, 175, 
		 225, 250, 350, 400, 400, 350, 250, 225, 
		 225, 250, 350, 400, 400, 350, 250, 225, 
		 175, 250, 300, 350, 350, 300, 250, 175, 
		 120, 200, 250, 250, 250, 250, 200, 120, 
		 50,  120, 175, 225, 225, 175, 120,  50},

		{50,  120, 175, 225, 225, 175, 120,  50, 
		 120, 200, 250, 250, 250, 250, 200, 120, 
		 175, 250, 300, 350, 350, 300, 250, 175, 
		 225, 250, 350, 400, 400, 350, 250, 225, 
		 225, 250, 350, 400, 400, 350, 250, 225, 
		 175, 250, 300, 350, 350, 300, 250, 175, 
		 120, 200, 250, 250, 250, 250, 200, 120, 
		 50,  120, 175, 225, 225, 175, 120,  50}
	},

	// Bishop
	{
		{150, 175, 200, 225, 225, 200, 175, 150, 
		 175, 225, 250, 300, 300, 250, 225, 175, 
		 200, 250, 300, 350, 350, 300, 250, 200, 
		 225, 300, 350, 350, 350, 350, 300, 225, 
		 225, 300, 350, 350, 350, 350, 300, 225, 
		 200, 250, 300, 350, 350, 300, 250, 200, 
		 175, 225, 250, 300, 300, 250, 225, 175, 
		 150, 175, 200, 225, 225, 200, 175, 150},

		{150, 175, 200, 225, 225, 200, 175, 150, 
		 175, 225, 250, 300, 300, 250, 225, 175, 
		 200, 250, 300, 350, 350, 300, 250, 200, 
		 225, 300, 350, 350, 350, 350, 300, 225, 
		 225, 300, 350, 350, 350, 350, 300, 225, 
		 200, 250, 300, 350, 350, 300, 250, 200, 
		 175, 225, 250, 300, 300, 250, 225, 175, 
		 150, 175, 200, 225, 225, 200, 175, 150}
	},

	// Rook
	{
		{300, 325, 350, 375, 375, 350, 325, 300, 
		 600, 600, 600, 600, 600, 600, 600, 600, 
		 450, 475, 500, 500, 500, 500, 475, 450, 
		 400, 425, 450, 475, 475, 450, 425, 400,  
		 300, 325, 350, 375, 375, 350, 325, 300, 
		 300, 275, 275, 275, 275, 275, 275, 275, 
		 350, 350, 350, 350, 350, 350, 350, 350, 
		 500, 500, 500, 500, 500, 500, 500, 500},

		{500, 500, 500, 500, 500, 500, 500, 500,
		 350, 350, 350, 350, 350, 350, 350, 350, 
		 300, 275, 275, 275, 275, 275, 275, 275, 
		 300, 325, 350, 375, 375, 350, 325, 300, 
		 400, 425, 450, 475, 475, 450, 425, 400, 
		 450, 475, 500, 500, 500, 500, 475, 450, 
		 600, 600, 600, 600, 600, 600, 600, 600, 
		 300, 325, 350, 375, 375, 350, 325, 300}
	}, 

	// Queen
	{
		{500, 550, 600, 650, 650, 600, 550, 500, 
		 825, 875, 900, 950, 950, 900, 875, 825, 
		 700, 775, 850, 900, 900, 850, 775, 700, 
		 675, 775, 850, 875, 875, 850, 775, 675, 
		 675, 775, 850, 875, 875, 850, 775, 675, 
		 550, 625, 700, 775, 775, 700, 625, 550, 
		 525, 550, 575, 625, 625, 575, 550, 500, 
		 550, 575, 600, 625, 625, 600, 575, 550}, 

		{700, 725, 750, 775, 775, 750, 725, 700, 
		 575, 625, 650, 700, 700, 650, 625, 575, 
		 550, 575, 625, 675, 675, 625, 575, 525, 
		 575, 675, 750, 775, 775, 750, 675, 575, 
		 675, 775, 850, 875, 875, 850, 775, 675, 
		 650, 725, 800, 875, 875, 800, 725, 650, 
		 675, 750, 800, 850, 850, 800, 750, 675, 
		 800, 825, 850, 875, 875, 850, 825, 800}
	},

	// King
	{
		{20,  10,  10,    0,   0,  10,  10,  20, 
		 20,  10,  10,    0,   0,  10,  10,  20, 
		 20,  10,  10,    0,   0,  10,  10,  20, 
		 20,  10,  10,    0,   0,  10,  10,  20, 
		 30,  20,  20,   10,  10,  20,  20,  30, 
		 40,  30,  30,   30,  30,  30,  30,  40, 
		 70,  70,  50,   50,  50,  50,  70,  70, 
		 70, 100,  80,   50,  50,  60, 100,  70},

		{70, 100,  80,  50,  50,  60, 100,  70, 
		 70,  70,  50,  50,  50,  50,  70,  70, 
		 40,  30,  30,  30,  30,  30,  30,  40, 
		 30,  20,  20,  10,  10,  20,  20,  30, 
		 20,  10,  10,   0,   0,  10,  10,  20, 
		 20,  10,  10,   0,   0,  10,  10,  20, 
		 20,  10,  10,   0,   0,  10,  10,  20, 
		 20,  10,  10,   0,   0,  10,  10,  20}}};

int pieceVal[] = {0, 10, 30, 35, 50, 90, 150};

char pieceNames[][10] = {"EMPTY", "PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING"};

int heuristic(GameState * state) {
	int scores[2] = {0, 0};


	// Evaluate current turn player
	uint64_t occupied = state->occupiedByColor[state->turn];
	uint64_t pawnBoard = 0;
	while(occupied) {
		int index = __builtin_ctzl(occupied);
		scores[state->turn] += pieceValues[(state->pieces[index] & PIECEMASK) - 1][state->turn][index];
		pawnBoard |= (uint64_t) ((state->pieces[index] & PIECEMASK) == PAWN) << index;
		occupied &= occupied - 1;
	}
	uint64_t doubledPawns = (pawnBoard << 8) & pawnBoard;
	doubledPawns |= (pawnBoard << 16) & pawnBoard;

	while(doubledPawns) {
		scores[state->turn] -= 75;
		doubledPawns &= doubledPawns - 1;
	}

	uint64_t blockedPawns = (pawnBoard << 8) & pawnBoard;
	while(blockedPawns) {
		scores[state->turn] -= 30;
		blockedPawns &= blockedPawns - 1;
	}


	// Evaluate next turn player
	occupied = state->occupiedByColor[!state->turn];
	pawnBoard = 0;
	while(occupied) {
		int index = __builtin_ctzl(occupied);
		scores[!state->turn] += pieceValues[(state->pieces[index] & PIECEMASK) - 1][!state->turn][index];
		pawnBoard |= (uint64_t) ((state->pieces[index] & PIECEMASK) == PAWN) << index;
		occupied &= occupied - 1;
	}

	doubledPawns = (pawnBoard >> 8) & pawnBoard;
	doubledPawns |= (pawnBoard >> 16) & pawnBoard;

	while(doubledPawns) {
		scores[!state->turn] -= 75;
		doubledPawns &= doubledPawns - 1;
	}

	blockedPawns = (pawnBoard >> 8) & pawnBoard;
	while(blockedPawns) {
		scores[!state->turn] -= 30;
		blockedPawns &= blockedPawns - 1;
	}

	return (scores[state->turn] - scores[!state->turn]);
}

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

int main(int argc, char * argv[]) {
	setpriority(PRIO_PROCESS, 0, -20);

	int depth;
	if(argc > 1) {
		depth = atoi(argv[1]);
	} else {
		depth = 7;
	}
	
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
			move = getComputerMove(&game, searchDepth);
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
}