#ifndef EVALUATION_HPP
#define EVALUATION_HPP


#include "Game.hpp"
#include "heuristic.hpp"

// typedef struct {
// 	int 
// }

int minMax(Game * game, int depth, int startDepth, int alpha, int beta, HeuristicFunction func);
Move iterativeDeepening(Game * game, int depth, HeuristicFunction func);


#endif