#include <cstdint>

#include "Magic.hpp"

#ifndef GAME_HPP
#define GAME_HPP

#define SQUARES 64

enum Piece : unsigned char {
	EMPTY =  0b0000,
	PAWN   = 0b0001,
	KNIGHT = 0b0010,
	BISHOP = 0b0011,
	ROOK   = 0b0100,
	QUEEN  = 0b0101,
	KING   = 0b0110,
	WHITE  = 0b0000,
	BLACK =  0b1000,
};

enum Flag {
	NONE 		   = 0b0000,
	PAWNDOUBLEJUMP = 0b0001,
	KINGMOVE       = 0b0010,
	RIGHTROOKMOVE  = 0b0011,
	LEFTROOKMOVE   = 0b0100,
	RIGHTCASTLE    = 0b0101,
	LEFTCASTLE     = 0b0110,
	ENPASSANT      = 0b0111,
	PROMOTE        = 0b1000,
	PROMOTEKNIGHT  = PROMOTE | KNIGHT,
	PROMOTEBISHOP  = PROMOTE | BISHOP,
	PROMOTEROOK    = PROMOTE | ROOK,
	PROMOTEQUEEN   = PROMOTE | QUEEN
};

typedef struct Move_t {
	uint8_t from;
	uint8_t to;
	Flag flag;
} Move;

typedef struct GameState_t {
	Piece pieces[64];
	uint64_t occupiedByColor[2];
	uint64_t occupied;
	uint64_t enPassant;
	int kingPosition[2];
	bool castling[2][2];
	int halfMove;
	int fullMove;
	bool turn;
} GameState;

class Game {
private:
	// Using a struct allows games to be easily copied for easier depth search.
	GameState state;

	void readMagicBitboards();

	uint64_t isPiecePinned(int position, bool color);
	bool isSquareSafe(int position, bool color);

	uint64_t getPawnAttacks(int position, bool color);
	uint64_t getKnightAttacks(int position);
	uint64_t getBishopAttacks(int position);
	uint64_t getRookAttacks(int position);
	uint64_t getQueenAttacks(int position);
	uint64_t getKingAttacks(int position);

	void getPawnMoves(int position, bool color, Move ** moveList);
	void getKnightMoves(int position, bool color, Move ** moveList);
	void getBishopMoves(int position, bool color, Move ** moveList);
	void getRookMoves(int position, bool color, Move ** moveList);
	void getQueenMoves(int position, bool color, Move ** moveList);
	void getKingMoves(int position, bool color, Move ** moveList);

public:
	Game(char input[150] = 0);
	void printBoard();
	void getLegalMoves(Move ** moveList);
	void printMoveList(Move * beginning, Move * end);
	void doMove(Move move);
};

#endif