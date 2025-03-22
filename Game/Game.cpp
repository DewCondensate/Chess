#include <iostream>
#include <cstring>
#include <stdlib.h>


#include "Game.hpp"

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
	state = stateIn;
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
	while(input[index] == 'k' || input[index] == 'Q' || input[index] == 'K' || input[index] == 'q' || input[index] == ' ' || input[index] == '-') {
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
	state.blockerMoves = getBlockingMoves(state.kingPosition[state.turn], state.turn);
}

void Game::printFen() {
	char letters[] = " PNBRQK";
	int counter = 0;
	for(int y = 0; y < 8; y++) {
		for(int x = 0; x < 8; x++) {
			if(state.pieces[convert(x, y)] == EMPTY) {
				counter++;
			} else {
				if(counter != 0) {
					printf("%d", counter);
					counter = 0;
				}
				printf("%c", (char) (letters[state.pieces[convert(x, y)]] + (state.occupiedByColor[1] >> convert(x, y) & 1) * ('a' - 'A')));
			}
		}
		if(counter != 0) {
			printf("%d", counter);
			counter = 0;
		}
		if(y != 7) {
			printf("/");
		}
	}
	printf(" ");

	printf("%c ", state.turn ? 'b' : 'w');

	char castling[2][2] = {{'Q', 'K'}, {'q', 'k'}};
	if(state.castling[0][0] + state.castling[0][1] + state.castling[1][0] + state.castling[1][1] > 2) {
		for(int i = 0; i < 2; i++) {
			for(int j = 1; j > -1; j--) {
				if(state.castling[i][j]) {
					printf("%c", castling[i][j]);
				}
			}
		}
		printf(" ");
	} else if(state.castling[0][0] + state.castling[0][1] + state.castling[1][0] + state.castling[1][1] > 0) {
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 2; j++) {
				if(state.castling[i][j]) {
					printf("%c", castling[i][j]);
				}
			}
		}
		printf(" - ");
	} else {
		printf("- ");
	}

	if(state.enPassant) {
		int pos = __builtin_ctzl(state.enPassant);
		printf("%c%c ", pos % 8 + 'a', (8 - pos / 8) + '0');
	}

	printf("%d ", state.halfMove);
	printf("%d\n", state.fullMove);
}

void Game::printBoard() {
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
	printf("\n\n");
}

uint64_t pawnMoves[2][64];
uint64_t pawnDoubleMoves[2][64];
uint64_t pawnAttacks[2][64];
uint64_t knightMoves[64];
uint64_t kingMoves[64];
uint64_t kingSpecialMoves[2][2];
uint64_t bishopFullMask[64];
uint64_t rookFullMask[64];
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

    bFile = fopen("bishopFullMasks.bits", "rb");
    rFile = fopen("rookFullMasks.bits", "rb");
    fread(rookFullMask, SQUARES, sizeof(uint64_t), rFile);
    fread(bishopFullMask, SQUARES, sizeof(uint64_t), bFile);
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
// VERY HOT PIECE OF CODE. OPTIMIZE THIS
uint64_t Game::isPiecePinned(int position, bool color) {
	// Empty, pawn, knight, bishop, rook, queen, king
	static bool pieceBoolsBish[] = {0, 0, 0, 1, 0, 1, 0};
	// Empty, pawn, knight, bishop, rook, queen, king
	static bool pieceBoolsRook[] = {0, 0, 0, 0, 1, 1, 0};
	static uint64_t bishopAttacks;
	static uint64_t rookAttacks;
	uint8_t kingPos = state.kingPosition[color];
	if((bishopAttacks = getBishopAttacks(kingPos) >> position & 1)) {
		Piece piece = BISHOP;
		Piece piece2 = QUEEN;
		// Checking if a bishop is on the same diagonal as the king and one other piece.
		bishopAttacks = bishopFullMask[kingPos] & state.occupiedByColor[!color] & getBishopAttacks(position);

		while(bishopAttacks) {
			if(pieceBoolsBish[state.pieces[__builtin_ctzl(bishopAttacks)]]) {
				// Returns the squares that a piece could move in to stay pinned without the king getting attacked.
				return (bishopMagics[position].mask & bishopMagics[kingPos].mask) | (ONE64 << __builtin_ctzl(bishopAttacks));
			}
			bishopAttacks &= bishopAttacks - 1;
		}
		return ALLSET;
	} else if((rookAttacks = getRookAttacks(kingPos) >> position & 1)) {
		// Checking if a rook is on the same diagonal as the king and one other piece.
		rookAttacks = rookFullMask[kingPos] & state.occupiedByColor[!color] & getRookAttacks(position);
		while(rookAttacks) {
			if(pieceBoolsRook[state.pieces[__builtin_ctzl(rookAttacks)]]) {
				// Returns the squares that a piece could move in to stay pinned without the king getting attacked.
				return (rookMagics[position].mask & rookMagics[kingPos].mask) | (ONE64 << __builtin_ctzl(rookAttacks));
			}
			rookAttacks &= rookAttacks - 1;
		}
		return ALLSET;
	}
	return ALLSET;
}

int Game::gameResult() {
	return (state.blockerMoves != ALLSET) * MIN_EVAL;
}

uint64_t Game::getBlockingMoves(int position, bool color) {
	uint64_t output = ~((uint64_t) 0);
	// Gets the pawn attacks from the square, then checks if there is an enemy pawn there
	uint64_t attacks = getPawnAttacks(position, color) & state.occupiedByColor[!color];
	Piece piece = PAWN;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece) {
			output &= (ONE64 << __builtin_ctzl(attacks));
		}
		attacks &= attacks - 1;
	}

	attacks = getKnightAttacks(position) & state.occupiedByColor[!color];
	piece = KNIGHT;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece) {
			output &= (ONE64 << __builtin_ctzl(attacks));
		}
		attacks &= attacks - 1;
	}

	uint64_t mask = getBishopAttacks(position);
	attacks = mask & state.occupiedByColor[!color];
	piece = BISHOP;
	Piece piece2 = QUEEN;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece || state.pieces[__builtin_ctzl(attacks)] == piece2) {
			output &= ((getBishopAttacks(__builtin_ctzl(attacks)) & mask) | (ONE64 << __builtin_ctzl(attacks)));
		}
		attacks &= attacks - 1;
	}

	mask = getRookAttacks(position);
	attacks = mask & state.occupiedByColor[!color];
	piece = ROOK;
	piece2 = QUEEN;

	while(attacks) {
		// state.pieces[__builtin_ctzl(attacks)] == piece || state.pieces[__builtin_ctzl(attacks)] == piece2;

		if(state.pieces[__builtin_ctzl(attacks)] == piece || state.pieces[__builtin_ctzl(attacks)] == piece2) {
			output = output;
			output &= ((getRookAttacks(__builtin_ctzl(attacks)) & mask) | (ONE64 << __builtin_ctzl(attacks)));
		}

		attacks &= attacks - 1;
	}


	attacks = getKingAttacks(position) & state.occupiedByColor[!color];
	piece = KING;
	while(attacks) {
		if(state.pieces[__builtin_ctzl(attacks)] == piece) {
			output &= (ONE64 << __builtin_ctzl(attacks));
		}
		attacks &= attacks - 1;
	}

	return output;
}

bool Game::isSquareSafe(int position, bool color) {
	static bool pieceBoolsBish[] = {0, 0, 0, 1, 0, 1, 0};
	// Empty, pawn, knight, bishop, rook, queen, king
	static bool pieceBoolsRook[] = {0, 0, 0, 0, 1, 1, 0};
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
	while(attacks) {
		if(pieceBoolsBish[state.pieces[__builtin_ctzl(attacks)]]) {
			return false;
		}
		attacks &= attacks - 1;
	}

	attacks = getRookAttacks(position) & state.occupiedByColor[!color];
	while(attacks) {
		if(pieceBoolsRook[state.pieces[__builtin_ctzl(attacks)]]) {
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

// Optimal
uint64_t Game::getPawnAttacks(int position, bool color) {
	return pawnAttacks[color][position];
}

// Optimal
uint64_t Game::getKnightAttacks(int position) {
	return knightMoves[position];
}

// Optimal
uint64_t Game::getBishopAttacks(int position) {
	uint64_t blockerBoard = (state.occupied) & bishopMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, bishopMagics[position]);
    return bishopAttackTable[position][index];
}

// Optimal
uint64_t Game::getRookAttacks(int position) {
	uint64_t blockerBoard = (state.occupied) & rookMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, rookMagics[position]);
    return rookAttackTable[position][index];
}

// Optimal
uint64_t Game::getQueenAttacks(int position) {
	return getRookAttacks(position) | getBishopAttacks(position);
}

// Optimal
uint64_t Game::getKingAttacks(int position) {
	return kingMoves[position];
}

// May be un-needed
inline Move createMove(uint8_t from, uint8_t to, Flag flag) {
	return {from, to, flag};
}

void Game::getPawnMoves(int position, bool color, Move ** moveList) {
	// printf("TURN %d %d %d\n", state.turn, position < 16, state.turn ? position < 16 : position >= 48);
	bool isPromotion = state.turn ? position >= 48 : position < 16;
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
	// printf("POSITION %d %d: \n", position/8, position % 8);
	// printUint(piecePinned);
	uint64_t output = pawnMoves[color][position] & ~state.occupied & piecePinned;
	if((output & state.blockerMoves)) {
		if(isPromotion) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEKNIGHT);
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEBISHOP);
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEROOK);
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEQUEEN);
		} else {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		}
	}
	if(output == pawnMoves[color][position]) {
		// Get rid of single jump move.
		output &= output - 1;
		output |= pawnDoubleMoves[color][position] & ~state.occupied & piecePinned & state.blockerMoves;
		// Check if double move is possible
		if(output) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PAWNDOUBLEJUMP);
		}
	}
	output = 0;

	output |= pawnAttacks[color][position] & (state.occupiedByColor[!color]) & piecePinned & state.blockerMoves;
	while(output) {
		if(isPromotion) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEKNIGHT);
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEBISHOP);
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEROOK);
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), PROMOTEQUEEN);
		} else {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		}
		output &= output - 1;
	}
	uint64_t blocker = state.turn ? (state.blockerMoves << 8) : (state.blockerMoves >> 8);
	blocker = state.enPassant == blocker ? ~((uint64_t) 0) : state.blockerMoves;
	output |= pawnAttacks[color][position] & state.enPassant & piecePinned & blocker;
	// Checking for en passant
	if(output) {
		// Special case if enPassant causes the king to be in check.
		// Remove the taking pawn from the board
		state.occupied &= ~(ONE64 << position);
		// Check if the pawn you are taking is "pinned" to the king
		if(isPiecePinned(__builtin_ctzl(state.enPassant) + (state.turn ? -8 : 8), color) & output) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(output), ENPASSANT);
		}
		// Add back the pawn so there are no weird effects
		state.occupied |= (ONE64 << position);
	}
}

void Game::getKnightMoves(int position, bool color, Move ** moveList) {
	uint64_t output = knightMoves[position] & ~state.occupiedByColor[color] & isPiecePinned(position, color) & state.blockerMoves;
	while(output) {
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		output &= output - 1;
	}
}

void Game::getBishopMoves(int position, bool color, Move ** moveList) {
	uint64_t blockerBoard = (state.occupied) & bishopMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, bishopMagics[position]);
    uint64_t output = bishopAttackTable[position][index] & ~state.occupiedByColor[color] & isPiecePinned(position, color) & state.blockerMoves;
	while(output) {
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), NONE);
		output &= output - 1;
	}
}

void Game::getRookMoves(int position, bool color, Move ** moveList) {
	uint64_t blockerBoard = (state.occupied) & rookMagics[position].mask;
	int index = getValueFromBlockerBoard(blockerBoard, rookMagics[position]);
    uint64_t output = rookAttackTable[position][index] & ~state.occupiedByColor[color] & isPiecePinned(position, color) & state.blockerMoves;
	while(output) {
		Flag flag = NONE;
		if(position == (color ? 0 : 56) || position == (color ? 7 : 63)) {
			flag = flagByIndex[rookPositions[position]];
		}
		*(*moveList)++ = createMove(position, __builtin_ctzl(output), flag);
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
	// Removing the king from the board temporarily so the bishops and rooks can "x-ray" through the king.
	state.occupied &= ~(ONE64 << state.kingPosition[state.turn]);
	while(tempMoves) {
		if(isSquareSafe(__builtin_ctzl(tempMoves), color)) {
			*(*moveList)++ = createMove(position, __builtin_ctzl(tempMoves), KINGMOVE);
		}
		tempMoves &= tempMoves - 1;
	}
	state.occupied |= (ONE64 << state.kingPosition[state.turn]);


	// Lots of testing to be done on castling branches
	if(position == 60 || position == 4/*OPTIMIZE TO THE FOLLOWING: && castling[color][0] | castling[color][1]*/ /*OR... && castling[color][0] || castling[color][1]*/) {
		if(state.castling[color][0]) {
			// This is super hacky. Just checking the square the king is moving to is safe and the square it hops over is safe
			if((kingSpecialMoves[color][0] & ~state.occupied) == kingSpecialMoves[color][0] && 
					isSquareSafe(position - 2, color) && 
					isSquareSafe(position - 1, color) && 
					state.blockerMoves == (~(uint64_t) 0) &&
					!(state.occupied >> (position - 3) & 1)) {
				*(*moveList)++ = createMove(position, position - 2, LEFTCASTLE);
			}
		}
		if(state.castling[color][1]) {
			// This is super hacky. Just checking the square the king is moving to is safe and the square it hops over is safe
			if((kingSpecialMoves[color][1] & ~state.occupied) == kingSpecialMoves[color][1] && 
					isSquareSafe(position + 2, color) && 
					isSquareSafe(position + 1, color) && 
					state.blockerMoves == (~(uint64_t) 0)) {
				*(*moveList)++ = createMove(position, position + 2, RIGHTCASTLE);
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


void printMove(Move move) {
	printf("%c%d%c%d", move.from % 8 + 'a', 7 - (move.from / 8) + 1, move.to % 8 + 'a', 7 - (move.to / 8) + 1);
}

int isCapture(GameState * state, Move move) {
	return (state->occupiedByColor[!state->turn] & (ONE64 << move.to));
}

void Game::printMoveList(Move * beginning, Move * end) {
	for(Move * i = beginning; i < end; i++) {
		printMove(*i);
		// printf("%c%d%c%d\n", i->from % 8 + 'a', 7 - (i->from / 8) + 1, i->to % 8 + 'a', 7 - (i->to / 8) + 1);
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
	// This reduces branching. It's ugly, but fast.
	static bool rookPositions[64] = {1, 0, 0, 0, 0, 0, 0, 1,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 1, 0, 0, 0, 0, 0, 0, 1};

	static bool rAO[64] = 			{1, 0, 0, 0, 0, 0, 0, 1,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0};

	static bool rAT[64] = 			{0, 0, 0, 0, 0, 0, 0, 1,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 1};

	if(rookPositions[move.to]) {
		state.castling[rAO[move.to]][rAT[move.to]] = false;
	}
	static char direction[2] = {-8, 8};
	// goto array?
	state.enPassant = 0;
	state.occupiedByColor[state.turn]  &= ~(ONE64 << move.from); // Remove 'ghost' of piece
	state.occupiedByColor[state.turn]  |=  (ONE64 << move.to); // Move piece to square
	state.occupiedByColor[!state.turn] &= ~(ONE64 << move.to);   // Take enemy piece

	state.pieces[move.to] = state.pieces[move.from];
	state.pieces[move.from] = EMPTY;

	switch(move.flag) {
		case NONE:
			break;
		case PAWNDOUBLEJUMP:
			state.enPassant = (ONE64 << (move.from + direction[state.turn]));
			break;
		case KINGMOVE:
			state.kingPosition[state.turn] = move.to;
			state.castling[state.turn][0] = false;
			state.castling[state.turn][1] = false;
			break;
		case LEFTROOKMOVE:
			state.castling[state.turn][0] = false;
			break;
		case RIGHTROOKMOVE:
			state.castling[state.turn][1] = false;
			break;
		case RIGHTCASTLE:
			state.kingPosition[state.turn] = move.to;
			state.occupiedByColor[state.turn] &= ~(ONE64 << (move.to + 1)); // Remove 'ghost' of piece
			state.occupiedByColor[state.turn] |=  (ONE64 << (move.to - 1));   // Take put piece there
			state.pieces[move.to - 1] = state.pieces[move.to + 1];
			state.pieces[move.to + 1] = EMPTY;
			state.castling[state.turn][0] = false;
			state.castling[state.turn][1] = false;
			break;
		case LEFTCASTLE:
			state.kingPosition[state.turn] = move.to;
			state.occupiedByColor[state.turn] &= ~(ONE64 << (move.to - 2)); // Remove 'ghost' of piece
			state.occupiedByColor[state.turn] |=  (ONE64 << (move.to + 1));   // Take put piece there
			state.pieces[move.to + 1] = state.pieces[move.to - 2];
			state.pieces[move.to - 2] = EMPTY;
			state.castling[state.turn][0] = false;
			state.castling[state.turn][1] = false;
			break;
		case ENPASSANT:
			state.occupiedByColor[!state.turn] &= ~(ONE64 << (move.to - direction[state.turn]));
			state.pieces[move.to - direction[state.turn]] = EMPTY;
			break;
		case PROMOTEQUEEN:
		case PROMOTEKNIGHT:
		case PROMOTEBISHOP:
		case PROMOTEROOK:
			// printf("PROMOTING TO: %d\n", move.flag & PIECEMASK);
			state.pieces[move.to] = (Piece) (move.flag & PIECEMASK);
			break;
		default:
			break;
	}
	state.occupied = state.occupiedByColor[0] | state.occupiedByColor[1];
	state.turn = !state.turn;
	state.blockerMoves = getBlockingMoves(state.kingPosition[state.turn], state.turn);
}

static char promotionChar[] = "  nbrq";
void printMove(Move move, uint64_t moves) {
	
	if(move.flag & PROMOTE) {
		printf("%c%d%c%d%c: %llu\n", move.from % 8 + 'a', 
				7 - (move.from / 8) + 1, 
				move.to % 8 + 'a', 
				7 - (move.to / 8) + 1, 
				promotionChar[move.flag & PIECEMASK], 
				moves);
	} else {
		printf("%c%d%c%d: %llu\n", move.from % 8 + 'a', 
				7 - (move.from / 8) + 1, 
				move.to % 8 + 'a', 7 - (move.to / 8) + 1, 
				moves);
	}
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

GameState Game::getGameState() {
	return state;
}

GameState * Game::getGameStatePtr() {
	return &state;
}


void Game::setGameState(GameState in) {
	state = in;
}

int max(int a, int b) {
	return a > b ? a : b;
}

int min(int a, int b) {
	return a < b ? a : b;
}


uint64_t Game::enumeratedPerft(int depth) {
	Move moveStart[MAXMOVESPOSSIBLE];
	Move * moves = moveStart;
	getLegalMoves(&moves);
	if(depth == 1) {
		for(Move * i = moveStart; i < moves; i++) {
			printMove(*i, 1);
		}
		printf("\nNodes searched: %ld\n", moves - moveStart);
		return moves - moveStart;
	}
	GameState redo = state;
	uint64_t moveCountTotal = 0;
	uint64_t tempMoveCount;
	for(Move * i = moveStart; i < moves; i++) {
		doMove(*i);
		tempMoveCount = 0;
		doPerft(depth - 1, &tempMoveCount);
		printMove(*i, tempMoveCount);
		moveCountTotal += tempMoveCount;
		state = redo;
	}

	printf("\nNodes searched: %llu\n\n", moveCountTotal);
	return moveCountTotal;
}