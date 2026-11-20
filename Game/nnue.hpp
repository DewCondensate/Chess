#ifndef NNUE_HPP
#define NNUE_HPP

#include <cstdint>
#include <cstring>

constexpr uint8_t BOARD_SIZE = 64;
constexpr uint8_t PIECE_COUNT = 13;
constexpr uint8_t LAYER_COUNT = 2;
constexpr uint8_t MAX_CONNECTIONS = 27;

enum Piece : uint8_t {
    EMPTY = 0,
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

struct MoveList {
    uint8_t count = 0;
    uint8_t squares[MAX_CONNECTIONS] = {};
};

struct Undo {
    uint8_t from;
    uint8_t to;
    uint8_t movedPiece;
    uint8_t capturedPiece;
};

class NeuralNet {
public:
	
};



#endif