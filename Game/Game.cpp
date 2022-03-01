#include <iostream>
#include <cstring>

#include "Game.hpp"

uint64_t ONE64 = 1;
uint64_t ALLSET = ~((uint64_t) 0);
const int MAXMOVESPOSSIBLE = 220;

int convert(int x, int y) {
	return y * 8 + x;
}

void printUint(uint64_t x) {
	for(int j = 0; j < 8; j++) {
		for(int i = 0; i < 8; i++) {
			printf("%c ", (x >> convert(i, j) & 1) ? '1' : '.');
		}
		printf("\n");
	}
	printf("\n");
}

Piece getPieceFromChar(char in) {
	in += in >= 'a' ? 'A' - 'a' : 0;
	switch(in) {
		case 'P':
			return PAWN;
		case 'N':
			return KNIGHT;
		case 'B':
			return BISHOP;
		case 'R':
			return ROOK;
		case 'Q':
			return QUEEN;
		case 'K':
			return KING;
		default:
			return EMPTY;
	}
}

Game::Game(GameState stateIn) {
	state = state;
}

unsigned char rookPositions[64] {0};
Flag flagByIndex[3] = {NONE, LEFTROOKMOVE, RIGHTROOKMOVE};
Game::Game(char inputString[150]) {
	rookPositions[0] = 1;
	rookPositions[7] = 2;
	rookPositions[56] = 1;
	rookPositions[63] = 2; 
	char input[150];
	if(inputString == 0) {
		strcpy(input, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	} else {
		strcpy(input, inputString);
	}
    state.enPassant = 0;
    for(uint8_t i = 0; i < SQUARES; i++) {
        state.pieces[i] = EMPTY;
    }
    state.castling[0][0] = 0;
    state.castling[0][1] = 0;
    state.castling[1][0] = 0;
    state.castling[1][1] = 0;
	state.occupiedByColor[0] = 0;
	state.occupiedByColor[1] = 0;
    state.occupied = 0;
    state.kingPosition[0] = 0;
    state.kingPosition[1] = 0;
    state.enPassant = 0;

	int y = 0;
	int x = 0;
	int index;
	char pieceChars[7] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};
	for(index = 0; input[index] != ' ' && y < 8; index++) {
		if(input[index] == 'K') {
			state.kingPosition[0] = convert(x, y);
		}
		if(input[index] == 'k') {
			state.kingPosition[1] = convert(x, y);
		}
		if(input[index] >= '0' && input[index] <= '9') {
			x += input[index] - '0';
			continue;
		} else if(input[index] >= 'A' && input[index] <= 'Z') {
			// This is a white piece.
			state.pieces[convert(x, y)] = getPieceFromChar(input[index]);
			state.occupiedByColor[0] |= ONE64 << convert(x, y);
			state.occupied |= ONE64 << convert(x, y);
		} else if(input[index] >= 'a' && input[index] <= 'z') {
			state.pieces[convert(x, y)] = getPieceFromChar(input[index]);
			state.occupiedByColor[1] |= ONE64 << convert(x, y);
			state.occupied |= ONE64 << convert(x, y);
		}

		if(x == 8 || input[index] == '/') {
			x = 0;
			y++;
		} else {
			x++;
		}
	}
	index++;
	state.turn = input[index] == 'b';
	index++;
	while(input[index] == 'k' || input[index] == 'Q' || input[index] == 'K' || input[index] == 'Q' || input[index] == ' ' || input[index] == '-') {
		switch(input[index]) {
			case 'k':
				state.castling[1][1] = true;
				break;
			case 'q':
				state.castling[1][0] = true;
				break;
			case 'K':
				state.castling[0][1] = true;
				break;
			case 'Q':
				state.castling[0][0] = true;
				break;
			default:
				break;
		}
		index++;
	}

	if(input[index] >= 'a' && input[index] <= 'z') {
		state.enPassant |= ONE64 << convert(input[index] - 'a', 8 - (input[index + 1] - '0'));
		index += 2;
	}

	state.halfMove = 0;
	while(input[index] >= '0' && input[index] <= '9') {
		state.halfMove *= 10;
		state.halfMove += input[index] - '0';
		index++;
	}
	index++;

	state.fullMove = 0;
	while(input[index] >= '0' && input[index] <= '9') {
		state.fullMove *= 10;
		state.fullMove += input[index] - '0';
		index++;
	}
	readMagicBitboards();
}

void Game::printBoard() {
	/*
	enum Piece {
		EMPTY =  0b0000,
		PAWN   = 0b0001,
		KNIGHT = 0b0010,
		BISHOP = 0b0011,
		ROOK   = 0b0100,
		QUEEN  = 0b0101,
		KING   = 0b0110,
		WHITE  = 0b0000,
		BLACK =  0b1000,
	};*/
	char pieceChars[12] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};
	for(int y = 0; y < 8; y++) {
		printf("  +–––+–––+–––+–––+–––+–––+–––+–––+\n");
		printf("%d ", 8 - y);
		for(int x = 0; x < 8; x++) {
			int colorShift = 0;
			if(state.occupiedByColor[1] >> convert(x, y) & 1 && state.pieces[convert(x, y)] != EMPTY) {
				colorShift = 'a' - 'A';
			}
			printf("| %c ", pieceChars[state.pieces[convert(x, y)]] + colorShift);
			// printf("| %d ", state.pieces[convert(x, y)]);
		}
		printf("|\n");
	}
	printf("  +–––+–––+–––+–––+–––+–––+–––+–––+\n    ");
	for(int i = 0; i < 8; i++) {
		printf("%c   ", 'a' + i);
	}
	printf("\n");
}

uint64_t pawnMoves[2][64];
uint64_t pawnDoubleMoves[2][64];
uint64_t pawnAttacks[2][64];
uint64_t knightMoves[64];
uint64_t kingMoves[64];
uint64_t kingSpecialMoves[2][2];
Magic rookMagics[64];
Magic bishopMagics[64];
uint64_t ** rookAttackTable;
uint64_t ** bishopAttackTable;
void Game::readMagicBitboards() {
	FILE * file = fopen("PawnMoves.bits", "rb");
	fread(pawnMoves, SQUARES * 2, sizeof(uint64_t), file);
	fread(pawnDoubleMoves, SQUARES * 2, sizeof(uint64_t), file);
	fread(pawnAttacks, SQUARES * 2, sizeof(uint64_t), file);
	fclose(file);

	file = fopen("KnightMoves.bits", "rb");
	fread(knightMoves, SQUARES, sizeof(uint64_t), file);
	fclose(file);


	FILE * rFile = fopen("rMagics.bits", "rb");
    FILE * bFile = fopen("bMagics.bits", "rb");
    for(uint8_t i = 0; i < SQUARES; i++) {
        rookMagics[i].readFromFile(rFile);
        bishopMagics[i].readFromFile(bFile);
    }
    fclose(rFile);
    fclose(bFile);

    rFile = fopen("rAttackTable.bits", "rb");
    bFile = fopen("bAttackTable.bits", "rb");
    uint32_t rSize;
    uint32_t bSize;
    rookAttackTable = (uint64_t **) malloc(SQUARES * sizeof(uint64_t *));
    bishopAttackTable = (uint64_t **) malloc(SQUARES * sizeof(uint64_t *));
    for(uint8_t i = 0; i < SQUARES; i++) {
        fread(&rSize, sizeof(uint32_t), 1, rFile);
        fread(&bSize, sizeof(uint32_t), 1, bFile);
        // std::cout << rSize << " " << bSize << std::endl;
        rookAttackTable[i] = (uint64_t *) malloc(rSize * sizeof(uint64_t));
        bishopAttackTable[i] = (uint64_t *) malloc(bSize * sizeof(uint64_t));
        fread(rookAttackTable[i], sizeof(uint64_t), rSize, rFile);
        fread(bishopAttackTable[i], sizeof(uint64_t), bSize, bFile);
    }
    fclose(rFile);
    fclose(bFile);


	file = fopen("KingMoves.bits", "rb");
	fread(kingMoves, SQUARES, sizeof(uint64_t), file);
	fread(kingSpecialMoves, 4, sizeof(uint64_t), file);
	fclose(file);
}

inline uint32_t getValueFromBlockerBoard(const uint64_t blockerBoard, Magic &magic) {
    return ((blockerBoard * magic.magic) >> (64 - magic.shift)); // THIS CAN BE OPTIMIZED, GET RID OF 64 - MAGIC.SHIFT.
}

// Returns the squares that a piece could move in to stay pinned without the king getting attacked.
uint64_t Game::isPiecePinned(int position, bool color) {
	if(getBishopAttacks(state.kingPosition[color]) >> position & 1) {
		Piece piece = BISHOP;
		Piece piece2 = QUEEN;
		// Checking if a bishop is on the same diagonal as the king and one other piece.
		uint64_t attacks = getBishopAttacks(position) & state.occupiedByColor[!color];
		while(attacks) {
			if(state.pieces[__builtin_ctzl(attacks)] == piece || state.pieces[__builtin_ctzl(attacks)] == piece2) {
				// Returns the squares that a piece could move in to stay pinned without the king getting attacked.
				return bishopMagics[position].mask & bishopMagics[state.kingPosition[color]].mask;
			}
			attacks &= attacks - 1;
		}
		return ALLSET;
	} else if(getRookAttacks(state.kingPosition[color]) >> position & 1) {
		Piece piece = ROOK;
		Piece piece2 = QUEEN;
		// Checking if a rook is on the same diagonal as the king and one other piece.
		uint64_t attacks = getRookAttacks(position) & state.occupiedByColor[!color];
		while(attacks) {
			if(state.pieces[__builtin_ctzl(attacks)] == piece || state.pieces[__builtin_ctzl(attacks)] == piece2) {
				// Returns the squares that a piece could move in to stay pinned without the king getting attacked.
				return rookMagics[position].mask & rookMagics[state.kingPosition[color]].mask;
			}
			attacks &= attacks - 1;
		}
		return ALLSET;
	}
	return ALLSET;
}

bool Game::isSquareSafe(int position, bool color) {
	// Gets the pawn attacks from the square, then checks if there is an enemy pawn there
	uint64_t attacks = getPawnAttacks(position, color) & state.occupiedByColor[!color];
	Piece piece = PAWN;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece) {
			return false;
		}
		attacks &= attacks - 1;
	}

	attacks = getKnightAttacks(position) & state.occupiedByColor[!color];
	piece = KNIGHT;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece) {
			return false;
		}
		attacks &= attacks - 1;
	}

	attacks = getBishopAttacks(position) & state.occupiedByColor[!color];
	piece = BISHOP;
	Piece piece2 = QUEEN;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece || state.pieces[__builtin_ctzl(attacks)] == piece2) {
			return false;
		}
		attacks &= attacks - 1;
	}

	attacks = getRookAttacks(position) & state.occupiedByColor[!color];
	piece = ROOK;
	piece2 = QUEEN;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece || state.pieces[__builtin_ctzl(attacks)] == piece2) {
			return false;
		}
		attacks &= attacks - 1;
	}

	attacks = getKingAttacks(position) & state.occupiedByColor[!color];
	piece = KING;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece) {
			return false;
		}
		attacks &= attacks - 1;
	}
	return true;
}

uint64_t Game::getPawnAttacks(int position, bool color) {
	return pawnAttacks[color][position];
}

uint64_t Game::getKnightAttacks(int position) {
	return knightMoves[position];
}

uint64_t Game::getBishopAttacks(int position) {
	uint64_t blockerBoard = (state.occupied) & bishopMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, bishopMagics[position]);
    return bishopAttackTable[position][index];
}

uint64_t Game::getRookAttacks(int position) {
	uint64_t blockerBoard = (state.occupied) & rookMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, rookMagics[position]);
    return rookAttackTable[position][index];
}

uint64_t Game::getQueenAttacks(int position) {
	return getRookAttacks(position) | getBishopAttacks(position);
}

uint64_t Game::getKingAttacks(int position) {
	return kingMoves[position];
}

Move createMove(uint8_t from, uint8_t to, Flag flag) {
	Move out = {from, to, flag};
	return out;
}

void Game::getPawnMoves(int position, bool color, Move ** moveList) {
	static uint64_t horizontalMasks[8] = {0xff00000000000000, 
								 0x00ff000000000000, 
								 0x0000ff0000000000,
								 0x000000ff00000000,
								 0x00000000ff000000,
								 0x0000000000ff0000,
								 0x000000000000ff00,
								 0x00000000000000ff};
	// There is probably some bit trick here OPTIMIZE
	uint64_t piecePinned = isPiecePinned(position, color);
	uint64_t output = pawnMoves[color][position] & ~state.occupied & piecePinned;
	if(output == pawnMoves[color][position]) {
		// Get rid of single jump move.
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		output &= output - 1;
		output |= pawnDoubleMoves[color][position] & ~state.occupied & piecePinned;
		// Check if double move is possible
		if(output) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PAWNDOUBLEJUMP);
			output &= output - 1;
		}
	}

	output |= pawnAttacks[color][position] & (state.occupiedByColor[!color]) & piecePinned;
	while(output) {
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		output &= output - 1;
	}

	output |= pawnAttacks[color][position] & state.enPassant & piecePinned;
	// Checking for en passant
	if(output) {
		// Special case if enPassant causes the king to be in check.
		// Remove the taking pawn from the board
		state.occupiedByColor[color] &= ~(ONE64 << position);
		state.occupied &= ~(ONE64 << position);
		// Check if the pawn you are taking is "pinned" to the king
		if(isPiecePinned(__builtin_ctzl(state.enPassant), color)) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), ENPASSANT);
		}
		// Add back the pawn so there are no weird effects
		state.occupiedByColor[color] |= (ONE64 << position);
		state.occupied |= (ONE64 << position);
	}
}

void Game::getKnightMoves(int position, bool color, Move ** moveList) {
	uint64_t output = knightMoves[position] & ~state.occupiedByColor[color] & isPiecePinned(position, color);
	while(output) {
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		output &= output - 1;
	}
}

void Game::getBishopMoves(int position, bool color, Move ** moveList) {
	uint64_t blockerBoard = (state.occupied) & bishopMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, bishopMagics[position]);
    uint64_t output = bishopAttackTable[position][index] & ~state.occupiedByColor[color] & isPiecePinned(position, color);
	while(output) {
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		output &= output - 1;
	}
}

void Game::getRookMoves(int position, bool color, Move ** moveList) {
	uint64_t blockerBoard = (state.occupied) & rookMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, rookMagics[position]);
    uint64_t output = rookAttackTable[position][index] & ~state.occupiedByColor[color] & isPiecePinned(position, color);
	while(output) {
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), flagByIndex[rookPositions[position]]);
		output &= output - 1;
	}
}

void Game::getQueenMoves(int position, bool color, Move ** moveList) {
	getBishopMoves(position, color, moveList);
	getRookMoves(position, color, moveList);
}

void Game::getKingMoves(int position, bool color, Move ** moveList) {
	uint64_t output;
	uint64_t tempMoves = kingMoves[position] & ~state.occupiedByColor[color];
	while(tempMoves) {
		if(isSquareSafe(__builtin_ctzl(tempMoves), color)) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(tempMoves), KINGMOVE);
		}
		tempMoves &= tempMoves - 1;
	}

	// Lots of testing to be done on castling branches
	if(position == 60/*OPTIMIZE TO THE FOLLOWING: && castling[color][0] | castling[color][1]*/ /*OR... && castling[color][0] || castling[color][1]*/) {
		if(state.castling[color][0]) {
			// This is super hacky. Just checking the square the king is moving to is safe and the square it hops over is safe
			if(kingSpecialMoves[color][0] & output == kingSpecialMoves[color][0] && isSquareSafe(position - 2, color)) {
				*(*moveList)++ = createMove(position - 2, __builtin_ctzl(output), LEFTCASTLE);
			}
		}
		if(state.castling[color][1]) {
			// This is super hacky. Just checking the square the king is moving to is safe and the square it hops over is safe
			if(kingSpecialMoves[color][1] & output == kingSpecialMoves[color][1] && isSquareSafe(position + 2, color)) {
				*(*moveList)++ = createMove(position - 2, __builtin_ctzl(output), RIGHTCASTLE);
			}
		}
	}
}

typedef void (Game::*GetMoveFunction) (int, bool, Move **);
void Game::getLegalMoves(Move ** moveList) {
	static GetMoveFunction functions[] = 
    {
    	&Game::getPawnMoves,
        &Game::getPawnMoves,
        &Game::getKnightMoves, 
        &Game::getBishopMoves,
        &Game::getRookMoves,
        &Game::getQueenMoves,
        &Game::getKingMoves
    };

	uint64_t pieces = state.occupiedByColor[state.turn];
	while(pieces) {
		GetMoveFunction toCall = functions[state.pieces[__builtin_ctzl(pieces)]];
		(this->*toCall)(__builtin_ctzl(pieces), state.turn, moveList);
		pieces &= pieces - 1;
	}
}

void Game::printMoveList(Move * beginning, Move * end) {
	for(Move * i = beginning; i < end; i++) {
		printf("%c%d%c%d\n", i->from % 8 + 'a', 7 - (i->from / 8) + 1, i->to % 8 + 'a', 7 - (i->to / 8) + 1);
	}
}

// enum Flag {
// 	NONE 		   = 0b0000,
// 	PAWNDOUBLEJUMP = 0b0001,
// 	KINGMOVE       = 0b0010,
// 	RIGHTROOKMOVE  = 0b0011,
// 	LEFTROOKMOVE   = 0b0100,
// 	RIGHTCASTLE    = 0b0101,
// 	LEFTCASTLE     = 0b0110,
// 	ENPASSANT      = 0b0111,
// 	PROMOTE        = 0b1000,
// 	PROMOTEKNIGHT  = PROMOTE | KNIGHT,
// 	PROMOTEBISHOP  = PROMOTE | BISHOP,
// 	PROMOTEROOK    = PROMOTE | ROOK,
// 	PROMOTEQUEEN   = PROMOTE | QUEEN
// };

// typedef struct GameState_t {
// 	Piece pieces[64]; 			 	// 64 bytes
// 	uint64_t occupiedByColor[2]; 	// 16 bytes
// 	uint64_t occupied;			 	// 8 bytes
// 	uint64_t enPassant;				// 8 bytes
// 	unsigned char kingPosition[2];	// 2 bytes
// 	bool castling[2][2];			// 4 bytes
// 	unsigned short halfMove;		// 2 bytes
// 	unsigned short fullMove;		// 2 bytes
// 	bool turn;						// 1 byte
// } GameState;

void Game::doMove(Move move) {
	static char direction[2] = {-8, 8};
	// goto array?
	state.enPassant = 0;
	state.occupiedByColor[state.turn]  &= ~(ONE64 << move.from); // Remove 'ghost' of piece
	state.occupiedByColor[state.turn]  |=  (ONE64 << move.from); // Move piece to square
	state.occupiedByColor[!state.turn] &= ~(ONE64 << move.to);   // Take enemy piece
	state.pieces[move.to] = state.pieces[move.from];
	state.pieces[move.from] = EMPTY;
	switch(move.flag) {
		case NONE:
			break;
		case PAWNDOUBLEJUMP:
			state.enPassant = ONE64 << (move.from + direction[state.turn]);
			break;
		case KINGMOVE:
			state.kingPosition[state.turn] = move.to;
			state.castling[state.turn][0] = false;
			state.castling[state.turn][1] = true;
		case RIGHTCASTLE:
		case LEFTCASTLE:
			state.castling[state.turn][move.flag & 1] = false;
			break;
		case ENPASSANT:
			state.occupiedByColor[!state.turn] &= ~(ONE64 << (move.to - direction[state.turn]));
			state.pieces[move.to - direction[state.turn]] = EMPTY;
			break;
		case PROMOTE:
			state.pieces[move.to] = (Piece) (move.flag & PIECEMASK);
			break;
	}
	state.occupied = state.occupiedByColor[0] | state.occupiedByColor[1];
	state.turn = !state.turn;
}

void Game::doPerft(int depth, uint64_t * moveCount) {
	Move moveStart[MAXMOVESPOSSIBLE];
	Move * moves = moveStart;
	getLegalMoves(&moves);
	GameState redo = state;
	if(depth == 1) {
		*moveCount += moves - moveStart;
		return;
	}

	for(Move * i = moveStart; i < moves; i++) {
		doMove(*i);
		doPerft(depth - 1, moveCount);
		state = redo;
	}
}

uint64_t Game::enumeratedPerft(int depth) {
	Move moveStart[MAXMOVESPOSSIBLE];
	Move * moves = moveStart;
	getLegalMoves(&moves);
	GameState redo = state;
	uint64_t moveCountTotal = 0;
	uint64_t tempMoveCount;
	for(Move * i = moveStart; i < moves; i++) {
		doMove(*i);
		printf("%c%d%c%d: ", i->from % 8 + 'a', 7 - (i->from / 8) + 1, i->to % 8 + 'a', 7 - (i->to / 8) + 1);
		tempMoveCount = 0;
		doPerft(depth - 1, &tempMoveCount);
		moveCountTotal += tempMoveCount;
		printf("%d\n", tempMoveCount);
		state = redo;
	}

	printf("Nodes: %d\n", moveCountTotal);
	return moveCountTotal;
}