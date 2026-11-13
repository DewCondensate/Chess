#include "Game.hpp"
#include "heuristic.hpp"

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