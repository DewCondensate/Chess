#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <chrono>

#include "../Magic.hpp"


// This program generates all the bitboards 


static const uint64_t ONE64 = 1;
static const uint32_t ONE32 = 1;
static const uint64_t ALLSET = ~0;
static const uint8_t SQUARES = 64;
static const uint8_t DIRSPERPIECE = 4;

enum Direction {
    NORTH =     0b1100,
    SOUTH =     0b0100,
    EAST =      0b0001,
    WEST =      0b0011,
    NORTHEAST = NORTH | EAST,
    NORTHWEST = NORTH | WEST,
    SOUTHEAST = SOUTH | EAST,
    SOUTHWEST = SOUTH | WEST
};

bool inBounds(int x, int y) {
	return x >= 0 && x < 8 && y >= 0 && y < 8;
}

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

// File layout:
// 64 uint64_t's of white pawn moves
// 64 uint64_t's of black pawn moves
// 64 uint64_t's of white double pawn moves
// 64 uint64_t's of black double pawn moves
// 64 uint64_t's of white attacking pawn moves
// 64 uint64_t's of black attacking pawn moves

void generatePawnMoves(FILE * file) {
	uint64_t output;
	int tempX;
	int tempY;
	int yDir;
	// i represents the color of the pawn, white is 0, black is 1.
	for(int i = 0; i < 2; i++) {
		for(int y = 0; y < 8; y++) {
			for(int x = 0; x < 8; x++) {
				output = 0;
				if(y != 0 && y != 7) {
					if(i == 0) {
						yDir = -1;
					} else {
						yDir = 1;
					}
					tempY = y + yDir;
					tempX = x;
					if(inBounds(tempX, tempY)) {
						output |= ONE64 << convert(tempX, tempY);
					}
				}
				fwrite(&output, 1, sizeof(uint64_t), file);
			}
		}
	}

    for(int i = 0; i < 2; i++) {
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                output = 0;
                if(y != 0 && y != 7) {
                    if(i == 0) {
                        yDir = -1;
                    } else {
                        yDir = 1;
                    }
                    tempY = y + yDir;
                    tempX = x;
                    if(y == (i == 0 ? 6 : 1)) { // If it's a double pawn move.
                        tempY += (i == 0 ? -1 : 1);
                        if(inBounds(tempX, tempY)) {
                            output |= ONE64 << convert(tempX, tempY);
                        }
                    }
                }
                fwrite(&output, 1, sizeof(uint64_t), file);
            }
        }
    }

	// i represents the color of the pawn, white is 0, black is 1.
	for(int i = 0; i < 2; i++) {
		for(int y = 0; y < 8; y++) {
			for(int x = 0; x < 8; x++) {
				output = 0;
				tempY = y + (i == 0 ? -1 : 1);
				for(int j = -1; j < 2; j += 2) {
					tempX = x + j;
					if(inBounds(tempX, tempY)) {
						output |= ONE64 << convert(tempX, tempY);
					}
				}
				fwrite(&output, 1, sizeof(uint64_t), file);
			}
		}
	}
}

// File layout:
// 64 uint64_t's of knight moves

void generateKnightMoves(FILE * file) {
	uint64_t output;
	int tempX;
	int tempY;
	int movesY[8] = {-1, -2, -2, -1,  1,  2,  2,  1};
	int movesX[8] = {-2, -1,  1,  2,  2,  1, -1, -2};
	for(int y = 0; y < 8; y++) {
		for(int x = 0; x < 8; x++) {
			output = 0;
			for(int move = 0; move < 8; move++) {
				tempY = y + movesY[move];
				tempX = x + movesX[move];
				if(inBounds(tempX, tempY)) {
					output |= ONE64 << convert(tempX, tempY);
				}
			}
			fwrite(&output, 1, sizeof(uint64_t), file);
		}
	}
}


// File layout:
// 64 uint64_t's of king moves
// 4 uint64_t's of special king moves. 
	//The special king moves include the square that the king hops over and the square the king is currently on.
void generateKingMoves(FILE * file) {
	uint64_t output;
	int tempX;
	int tempY;
	int movesY[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
	int movesX[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    for(int y = 0; y < 8; y++) {
	   for(int x = 0; x < 8; x++) {
			output = 0;
			for(int move = 0; move < 8; move++) {
				tempY = y + movesY[move];
				tempX = x + movesX[move];
				if(inBounds(tempX, tempY)) {
					output |= ONE64 << convert(tempX, tempY);
				}
			}
			fwrite(&output, 1, sizeof(uint64_t), file);
            printUint(output);
		}
	}

    // Hard coding castling.
    output = 0;
	output |= ONE64 << convert(4 - 1, 7);
    output |= ONE64 << convert(4 - 2, 7);
    fwrite(&output, 1, sizeof(uint64_t), file);
    output = 0;
	output |= ONE64 << convert(4 + 1, 7);
	output |= ONE64 << convert(4 + 2, 7);
    fwrite(&output, 1, sizeof(uint64_t), file);
    output = 0;
    output |= ONE64 << convert(4 - 1, 0);
    output |= ONE64 << convert(4 - 2, 0);
    fwrite(&output, 1, sizeof(uint64_t), file);
    output = 0;
    output |= ONE64 << convert(4 + 1, 0);
    output |= ONE64 << convert(4 + 2, 0);
    fwrite(&output, 1, sizeof(uint64_t), file);
}

uint8_t getRow(const uint8_t index) {return index / 8;}
uint8_t getCol(const uint8_t index) {return index % 8;}
int8_t getIndex(const uint8_t row, const uint8_t column) {return row * 8 + column;}

int8_t getNextIndex(const uint8_t index, const Direction dir) {
    int8_t row = getRow(index);
    int8_t column = getCol(index);
    row += ((dir >> 3 & 1) ? -1 : 1) * (dir >> 2 & 1);
    column += ((dir >> 1 & 1) ? -1 : 1) * (dir & 1);
    return ((row >= 0 && row < 8 && column >= 0 && column < 8) ? getIndex(row, column) : -1);
}

void genBB(const uint16_t index, const uint64_t mask, uint64_t * output) {
    *output = 0;
    uint8_t count = 0;
    for(uint8_t i = 0; i < SQUARES; i++) {
        if(mask >> i & 1) {
            if(index >> count & 1) {
                *output |= ONE64 << i;
            }
            count++;
        }
    }
}

void getBBs(const uint64_t mask, uint16_t size, uint64_t * output) {
    for(uint16_t i = 0; i < size; i++) {
        genBB(i, mask, &output[i]);
    }
}

void solveBB(const uint8_t position, const uint64_t bb, const Direction dirs[4], uint64_t * output) {
    *output = 0;
    int8_t index;
    for(uint8_t i = 0; i < DIRSPERPIECE; i++) {
        index = position;
        while((index = getNextIndex(index, dirs[i])) != -1) {
            *output |= ONE64 << index;
            if(bb >> index & 1) {
                break;
            }
        }
    }
}

void solveBBs(const uint16_t size, const uint8_t position, const uint64_t * bbs, const Direction dirs[4], uint64_t * output) {
    for(uint16_t i = 0; i < size; i++) {
        solveBB(position, bbs[i], dirs, &output[i]);
    }
}

void getFullMask(const uint8_t position, const Direction dirs[4], uint64_t * mask) {
    *mask = 0;
    int8_t index;
    for(uint8_t i = 0; i < DIRSPERPIECE; i++) {
        index = position;
        while((index = getNextIndex(index, dirs[i])) != -1) {
            *mask |= ONE64 << index;
        }
    }
}

void getMask(const uint8_t position, const Direction dirs[4], uint64_t * mask, uint16_t * shift) {
    *mask = 0;
    int8_t index;
    *shift = 0;
    for(uint8_t i = 0; i < DIRSPERPIECE; i++) {
        index = position;
        while((index = getNextIndex(index, dirs[i])) != -1
        && (getNextIndex(index, dirs[i]) != -1)) {
            *mask |= ONE64 << index;
            (*shift)++;
        }
    }
}

inline uint64_t rand64() {
    return (( ( ( (uint64_t) rand() ) & 0xffff) << 48) | 
            ( ( ( (uint64_t) rand() ) & 0xffff) << 32) | 
            ( ( ( (uint64_t) rand() ) & 0xffff) << 16) | 
            ( (   (uint64_t) rand() ) & 0xffff) );
}

inline uint64_t zeroedRand64() {
    return rand64() & rand64() & rand64();
}

inline uint16_t indexFromMagic(const uint64_t bb, const uint64_t magic, const uint8_t shift) {
    return ((bb * magic) >> (64 - shift));
}

void getMagic(const uint64_t * bb, const uint64_t * sBB, const uint16_t size, const uint8_t shift, uint64_t * magic, uint64_t * table) {
    bool collision;
    uint16_t largestIndex;
    uint64_t testingMagic;
    while(1) {
        testingMagic = zeroedRand64();
        largestIndex = 0;
        collision = false;
        for(uint16_t i = 0; i < size; i++) {
            table[i] = ALLSET;
        }
        for(uint16_t i = 0; i < size; i++) {
            uint16_t index = indexFromMagic(bb[i], testingMagic, shift);
            if(table[index] == ALLSET || table[index] == sBB[i]) {
                table[index] = sBB[i];
                largestIndex = i > largestIndex ? i : largestIndex;
            } else {
                collision = true;
                break;
            }
        }
        if(!collision) {
            *magic = testingMagic;
            break;
        }
    }
}

void generateRookAndBishopMagics() {
    srand(time(0));
    static const Direction rDirections[] = {NORTH, SOUTH, EAST, WEST};
    static const Direction bDirections[] = {NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST};

    uint64_t ** rBBs = (uint64_t **) malloc(SQUARES * sizeof(uint64_t));
    uint64_t ** bBBs = (uint64_t **) malloc(SQUARES * sizeof(uint64_t));
    uint64_t ** rBBsSolved = (uint64_t **) malloc(SQUARES * sizeof(uint64_t));
    uint64_t ** bBBsSolved = (uint64_t **) malloc(SQUARES * sizeof(uint64_t));

    uint64_t ** rookAttackTable = (uint64_t **) malloc(SQUARES * sizeof(uint64_t *));
    uint64_t ** bishopAttackTable = (uint64_t **) malloc(SQUARES * sizeof(uint64_t *));

    uint32_t rookSize[64];
    uint32_t bishopSize[64];


    Magic rMagics[SQUARES];
    Magic bMagics[SQUARES];

    FILE * rFile;
    FILE * bFile;
    rFile = fopen("rookFullMasks.bits", "wb");
    bFile = fopen("bishopFullMasks.bits", "wb");
    uint64_t rMask;
    uint64_t bMask;
    for(int i = 0; i < 64; i++) {
        getFullMask(i, bDirections, &bMask);
        getFullMask(i, rDirections, &rMask);
        fwrite(&rMask, 1, sizeof(uint64_t), rFile);
        fwrite(&bMask, 1, sizeof(uint64_t), bFile);
    }
    fclose(rFile);
    fclose(bFile);
    rFile = fopen("rAttackTable.bits", "wb");
    bFile = fopen("bAttackTable.bits", "wb");
    for(uint8_t i = 0; i < SQUARES; i++) {
        getMask(i, rDirections, &rMagics[i].mask, &rMagics[i].shift);
        rookSize[i] = (ONE32 << rMagics[i].shift);
        rBBs[i] = (uint64_t *) malloc(rookSize[i] * sizeof(uint64_t));
        getBBs(rMagics[i].mask, rookSize[i], rBBs[i]);
        rBBsSolved[i] = (uint64_t *) malloc(rookSize[i] * sizeof(uint64_t));
        solveBBs(rookSize[i], i, rBBs[i], rDirections, rBBsSolved[i]);
        rookAttackTable[i] = (uint64_t *) malloc(rookSize[i] * sizeof(uint64_t));
        printf("Getting rook magic %d/%d\n", (i+1), SQUARES);
        getMagic(rBBs[i], rBBsSolved[i], rookSize[i], rMagics[i].shift, &rMagics[i].magic, rookAttackTable[i]);
        fwrite(&rookSize[i], sizeof(uint32_t), 1, rFile);
        fwrite(rookAttackTable[i], sizeof(uint64_t), rookSize[i], rFile);
    

        getMask(i, bDirections, &bMagics[i].mask, &bMagics[i].shift);
        bishopSize[i] = (ONE32 << bMagics[i].shift);
        bBBs[i] = (uint64_t *) malloc(bishopSize[i] * sizeof(uint64_t));
        getBBs(bMagics[i].mask, bishopSize[i], bBBs[i]);
        bBBsSolved[i] = (uint64_t *) malloc(bishopSize[i] * sizeof(uint64_t));
        solveBBs(bishopSize[i], i, bBBs[i], bDirections, bBBsSolved[i]);
        bishopAttackTable[i] = (uint64_t *) malloc(bishopSize[i] * sizeof(uint64_t));
        printf("Getting bishop magic %d/%d\n", (i+1), SQUARES);
        getMagic(bBBs[i], bBBsSolved[i], bishopSize[i], bMagics[i].shift, &bMagics[i].magic, bishopAttackTable[i]);
        fwrite(&bishopSize[i], sizeof(uint32_t), 1, bFile);
        fwrite(bishopAttackTable[i], sizeof(uint64_t), bishopSize[i], bFile);
    }
    fclose(rFile);
    fclose(bFile);

    for(int i = 0; i < 8; i++) {
        printf("\n");
        for(int j = 0; j < 8; j++) {
        	printf("%d ", rMagics[i*8+j].shift);
        }
    }

    // printf("\n");
    // for(int i = 0; i < 8; i++) {
    // 	printf("\n");
    //     for(int j = 0; j < 8; j++) {
    //         printf("%d ", rMagics[i*8+j].magic);
    //     }
    // }
    // printf("\n");

    for(int i = 0; i < 8; i++) {
        printf("\n");
        for(int j = 0; j < 8; j++) {
            printf("%d ", bMagics[i*8+j].shift);
        }
    }
    printf("\n");

    // for(int i = 0; i < 8; i++) {
    //     printf("\n");
    //     for(int j = 0; j < 8; j++) {
    //         printf("%d ", bMagics[i*8+j].magic);
    //     }
    // }
    // printf("\n");

    rFile = fopen("rMagics.bits", "wb");
    bFile = fopen("bMagics.bits", "wb");
    for(int i = 0; i < SQUARES; i++) {
        rMagics[i].writeToFile(rFile);
        bMagics[i].writeToFile(bFile);
    }
    fclose(rFile);
    fclose(bFile);
    printf("Done!\n");
}

int main() {
	FILE * file;
	
	file = fopen("PawnMoves.bits", "w");
	generatePawnMoves(file);
	fclose(file);
	
	file = fopen("KnightMoves.bits", "w");
	generateKnightMoves(file);
	fclose(file);

    generateRookAndBishopMagics();
	
	file = fopen("KingMoves.bits", "w");
	generateKingMoves(file);
	fclose(file);
}