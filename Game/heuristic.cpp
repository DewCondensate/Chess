#include "Game.hpp"
#include "heuristic.hpp"

int Heuristic::heuristicFullCalculate(GameState * state) {
	// Evaluate current turn player
	uint64_t occupied = state->occupiedByColor[state->turn];
	uint64_t pawnBoard = 0;
	scores[0] = 0;
	scores[1] = 0;
	while(occupied) {
		int index = __builtin_ctzl(occupied);
		scores[state->turn] += pieceValues[(state->pieces[index] & PIECEMASK) - 1][state->turn][index];
		pawnBoard |= (uint64_t) ((state->pieces[index] & PIECEMASK) == PAWN) << index;
		occupied &= occupied - 1;
	}
	uint64_t doubledPawns = (pawnBoard << 8) & pawnBoard;
	doubledPawns |= (pawnBoard << 16) & pawnBoard;

	// while(doubledPawns) {
	// 	scores[state->turn] -= 75;
	// 	doubledPawns &= doubledPawns - 1;
	// }

	// uint64_t blockedPawns = (pawnBoard << 8) & pawnBoard;
	// while(blockedPawns) {
	// 	scores[state->turn] -= 30;
	// 	blockedPawns &= blockedPawns - 1;
	// }


	// Evaluate next turn player
	occupied = state->occupiedByColor[!state->turn];
	pawnBoard = 0;
	while(occupied) {
		int index = __builtin_ctzl(occupied);
		scores[!state->turn] += pieceValues[(state->pieces[index] & PIECEMASK) - 1][!state->turn][index];
		pawnBoard |= (uint64_t) ((state->pieces[index] & PIECEMASK) == PAWN) << index;
		occupied &= occupied - 1;
	}

	// doubledPawns = (pawnBoard >> 8) & pawnBoard;
	// doubledPawns |= (pawnBoard >> 16) & pawnBoard;

	// while(doubledPawns) {
	// 	scores[!state->turn] -= 75;
	// 	doubledPawns &= doubledPawns - 1;
	// }

	// blockedPawns = (pawnBoard >> 8) & pawnBoard;
	// while(blockedPawns) {
	// 	scores[!state->turn] -= 30;
	// 	blockedPawns &= blockedPawns - 1;
	// }

	output = (scores[state->turn] - scores[!state->turn]);
	// printf("Full output %d, scores1: %d, scores2 %d\n", output, scores[state->turn], scores[!state->turn]);
	return output;
}

#ifdef EFFICIENT_UPDATE_HEURISTIC


void Heuristic::heuristicDoMove(bool turn, ToFrom* toFrom) {

    const int mover = turn;
    const int opponent = !mover;

    for (int i = 0; i < toFrom->count; i++) {

        const int from = toFrom->from[i];
        const int to = toFrom->to[i];
        const uint8_t piece = toFrom->oldPiece[i];

        if (to == 65) {
            scores[opponent] -=
                pieceValues[(piece & PIECEMASK) - 1]
                           [opponent][from];
            continue;
        }

		// printf("SCORE BEFORE: %d\n", scores[mover]);

        scores[mover] -=
            pieceValues[(piece & PIECEMASK) - 1]
                       [mover][from];

        const uint8_t newPiece =
            toFrom->promote[i] ? toFrom->promote[i] : piece;

        scores[mover] +=
            pieceValues[(newPiece & PIECEMASK) - 1]
                       [mover][to];

		// printf("MOVE: %d -> %d\n", from, to);
		// printf("PIECE: %d\n", piece);
		// printf("OLD VALUE: %d\n",
		// 	pieceValues[(piece & PIECEMASK) - 1][mover][from]);
		// printf("NEW VALUE: %d\n",
		// 	pieceValues[(newPiece & PIECEMASK) - 1][mover][to]);
		// printf("SCORE AFTER: %d\n", scores[mover]);
    }
    output = scores[opponent] - scores[mover];
	// printf("Output %d, scores1: %d, scores2 %d\n", output, scores[opponent], scores[mover]);
}


void Heuristic::heuristicUndoMove(bool turn, ToFrom* toFrom) {

    const int mover = turn;
    const int opponent = !mover;

    for (int i = 0; i < toFrom->count; i++) {

        const int from = toFrom->from[i];
        const int to = toFrom->to[i];
        const uint8_t piece = toFrom->oldPiece[i];

        if (to == 65) {
            scores[opponent] +=
                pieceValues[(piece & PIECEMASK) - 1]
                           [opponent][from];
            continue;
        }

        const uint8_t newPiece =
            toFrom->promote[i] ? toFrom->promote[i] : piece;

        scores[mover] -=
            pieceValues[(newPiece & PIECEMASK) - 1]
                       [mover][to];

        scores[mover] +=
            pieceValues[(piece & PIECEMASK) - 1]
                       [mover][from];
    }

    output = scores[mover] - scores[opponent];
}

int Heuristic::getOutput() {
	return output;
}

NNUEHeuristic::NNUEHeuristic() {
    
}

#endif