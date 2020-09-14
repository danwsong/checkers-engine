//
//  engine.h
//  checkers
//
//  Created by Dan Song on 2020-06-05.
//  Copyright © 2020 daniel song. All rights reserved.
//

#ifndef engine_h
#define engine_h

#include "board.h"

#define INFINITY    1000000.0
#define DEPTH       15

double evaluate(board_t board);
double evaluate_move(board_t board, move_t move);
double minimax(board_t board, int depth, double alpha, double beta, move_t * move);

#endif /* engine_h */
