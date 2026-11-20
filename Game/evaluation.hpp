#ifndef EVALUATION_HPP
#define EVALUATION_HPP


#include "Game.hpp"
#include "heuristic.hpp"

int minMax(Game * game, int depth, int startDepth, int alpha, int beta, Heuristic * heuristic);
Move iterativeDeepening(Game * game, int depth, Heuristic * func);

#endif