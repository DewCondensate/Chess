#include "evaluation.hpp"
#include "Game.hpp"

#define MAX(a, b) (a > b ? a : b)

int minMax(Game * game, int depthleft, int startDepth, int alpha, int beta, Heuristic * heuristic) {
	#ifdef EFFICIENT_UPDATE_HEURISTIC
	ToFrom toFrom;
	#endif

	GameState state = game->getGameState();
	if(depthleft == 0) {
		#ifdef EFFICIENT_UPDATE_HEURISTIC
		return heuristic->getOutput();
		#else
		return heuristic->heuristicFullCalculate(&state);
		#endif
	}

	Move moveStart[MAXMOVESPOSSIBLE];
    Move * moves = moveStart;

	int bestValue = INT_MIN;
    game->getLegalMoves(&moves);

    if(moves == (Move *) &moveStart) {
		return game->gameResult();
	}


	for(Move * i = moveStart; i < moves; i++) {
		#ifdef EFFICIENT_UPDATE_HEURISTIC
        	game->doMove(*i, &toFrom);
			heuristic->heuristicDoMove(state.turn, &toFrom);

			// int incremental = heuristic->getOutput();

			// GameState state2 = game->getGameState();

			// Heuristic test;
			// int full = test.heuristicFullCalculate(&state2);

			// if (incremental != full) {
			// 	printf("HEURISTIC MISMATCH in minmax %d: incremental=%d full=%d\n",
			// 		depthleft, incremental, full);
			// 	printf("To: %d, from %d\n", toFrom.to[0], toFrom.from[0]);
			// 	return 1;
			// }
		#else
        	game->doMove(*i);
		#endif
		int tempScore = -minMax(game, depthleft - 1, startDepth, -beta, -alpha, heuristic);
		#ifdef EFFICIENT_UPDATE_HEURISTIC
			heuristic->heuristicUndoMove(state.turn, &toFrom);
		#endif
        
		game->setGameState(state);

		bestValue = MAX(bestValue, tempScore);

		alpha = MAX(alpha, bestValue);
		if(alpha > beta) {
			break;
		}
	}

	return bestValue;
}

struct IterativeDeepening {
	Move moves[MAXMOVESPOSSIBLE];
	int eval[MAXMOVESPOSSIBLE];
};


void swapInt(int * a, int * b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void swapMove(Move * a, Move * b) {
    Move temp = *a;
    *a = *b;
    *b = temp;
}

int partition(Move moves[], int eval[], int low, int high) {
    // Initialize pivot to be the first element
    int p = eval[low];
    int i = low;
    int j = high;

    while (i < j) {
        // Find the first element smaller than
        // the pivot (from starting)
        while (eval[i] >= p && i <= high - 1) {
            i++;
        }

        // Find the first element greater than
        // the pivot (from last)
        while (eval[j] < p && j >= low + 1) {
            j--;
        }

        if (i < j) {
            swapInt(&eval[i], &eval[j]);
            swapMove(&moves[i], &moves[j]);
        }
    }

    swapInt(&eval[low], &eval[j]);
    swapMove(&moves[low], &moves[j]);

    return j;
}

void quickSort(Move moves[], int eval[], int low, int high) {
    if (low < high) {
        int pi = partition(moves, eval, low, high);

        quickSort(moves, eval, low, pi - 1);
        quickSort(moves, eval, pi + 1, high);
    }
}

// int iterativeEval(Game * game, int depth, HeuristicFunction func) {
// 	GameState state = game->getGameState();

// 	IterativeDeepening moves;

// 	Move * moveEnd = moves.moves;
// 	game->getLegalMoves(&moveEnd);
// 	int prevBest = 0;

// 	for(int i = depth - 2; i < depth; i++) {
// 		int bestEval = MIN_EVAL;
// 		for(int j = 0; j < moveEnd - moves.moves; j++) {
// 			if(moves.eval[j] < prevBest - (prevBest / 2)) {
// 				break;
// 			}
// 			game->doMove(moves.moves[j]);
// 			moves.eval[j] = -minMax(game, i-1, i-1, MIN_EVAL, -MIN_EVAL, func);
// 			bestEval = MAX(bestEval, moves.eval[j]);
// 			game->setGameState(state);
// 		}

// 		quickSort(moves.moves, moves.eval, 0, moveEnd - moves.moves - 1);
// 		prevBest = moves.eval[0];
// 	}

// 	return moves.eval[0];
// }

#define ABS(x) (x < 0 ? -x : x)
Move iterativeDeepening(Game * game, int depth, Heuristic * heuristic) {
	#ifdef EFFICIENT_UPDATE_HEURISTIC
	ToFrom toFrom;
	#endif

	GameState state = game->getGameState();
	heuristic->heuristicFullCalculate(&state);

	IterativeDeepening moves;

	Move * moveEnd = moves.moves;
	game->getLegalMoves(&moveEnd);

	int bestEval = MIN_EVAL;
	for(int j = 0; j < moveEnd - moves.moves; j++) {
		#ifdef EFFICIENT_UPDATE_HEURISTIC
        	game->doMove(moves.moves[j], &toFrom);
			// printf("Current output: %d\n", heuristic->getOutput());
			heuristic->heuristicDoMove(state.turn, &toFrom);

			// int incremental = heuristic->getOutput();

			// Heuristic test;
			// GameState state2 = game->getGameState();
			// int full = test.heuristicFullCalculate(&state2);

			// if (incremental != full) {
			// 	printf("HEURISTIC MISMATCH in iterative deepening %d: incremental=%d full=%d\n",
			// 		j, incremental, full);
			// 	printf("To: %d, from %d\n", toFrom.to[0], toFrom.from[0]);
			// 	// break;
			// 	}
		#else
        	game->doMove(moves.moves[j]);
		#endif
		moves.eval[j] = -minMax(game, depth - 2, depth - 2, MIN_EVAL, -MIN_EVAL, heuristic);
		#ifdef EFFICIENT_UPDATE_HEURISTIC
			heuristic->heuristicUndoMove(state.turn, &toFrom);
		#endif

		bestEval = MAX(bestEval, moves.eval[j]);
		game->setGameState(state);
	}

	quickSort(moves.moves, moves.eval, 0, moveEnd - moves.moves - 1);
	int prevBest = moves.eval[0];

	for(int j = 0; j < moveEnd - moves.moves; j++) {
		printMove(moves.moves[j]);
    	printf(": %d\n", moves.eval[j]);
	}

	printf("Best move: ");
	printMove(moves.moves[0]);
	printf(": %d\n", moves.eval[0]);

	printf("REPEATING\n\n");

	for(int j = 0; j < moveEnd - moves.moves; j++) {
		if(moves.eval[j] < prevBest - abs(prevBest)) {
			printf("%d, %d\n", moves.eval[j], prevBest);
			quickSort(moves.moves, moves.eval, 0, moveEnd - moves.moves - 1);
			for(int k = 0; k < j; k++) {
				printMove(moves.moves[k]);
	        	printf(": %d\n", moves.eval[k]);
			}
			return moves.moves[0];
		}

		#ifdef EFFICIENT_UPDATE_HEURISTIC
        	game->doMove(moves.moves[j], &toFrom);
			heuristic->heuristicDoMove(state.turn, &toFrom);
		#else
        	game->doMove(moves.moves[j]);
		#endif
		moves.eval[j] = -minMax(game, depth - 1, depth - 1, MIN_EVAL, -MIN_EVAL, heuristic);
		#ifdef EFFICIENT_UPDATE_HEURISTIC
			heuristic->heuristicUndoMove(state.turn, &toFrom);
		#endif

		bestEval = MAX(bestEval, moves.eval[j]);
		game->setGameState(state);
	}

	quickSort(moves.moves, moves.eval, 0, moveEnd - moves.moves - 1);
	for(int j = 0; j < moveEnd - moves.moves; j++) {
		printMove(moves.moves[j]);
    	printf(": %d\n", moves.eval[j]);
	}


	printf("Best move: ");
	printMove(moves.moves[0]);
	printf(": %d\n", moves.eval[0]);
	return moves.moves[0];
}