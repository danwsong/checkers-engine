//
//  board.h
//  checkers
//
//  Created by Dan Song on 2020-06-04.
//  Copyright © 2020 daniel song. All rights reserved.
//

#ifndef board_h
#define board_h

#include <stdint.h>

#include "bitboard.h"
#include "move.h"

#define NUM_SIDES   2
#define NUM_PIECES  2
#define NUM_SQUARES 32

#define BLACK       0
#define RED         1

#define MAN         0
#define KING        1

#define LEFT_EDGE   0xE0E0E0E0
#define RIGHT_EDGE  0x07070707
#define BACK_ROW    0xF0000000
#define FRONT_ROW   0x0000000F

typedef uint32_t bitboard_t;

typedef struct {
    bitboard_t sides[NUM_SIDES];
    bitboard_t kings;
    int turn;
    int moves_since_last_capture;
    int game_over;
    int result;
} board_t;

extern board_t starting_board;

int generate_legal_moves(board_t board, move_t * moves);

board_t move_piece(board_t board, move_t move);

#endif /* board_h */
