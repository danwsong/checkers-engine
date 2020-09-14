//
//  board.c
//  checkers
//
//  Created by Dan Song on 2020-06-04.
//  Copyright © 2020 daniel song. All rights reserved.
//

#include "board.h"

board_t starting_board = {
    .sides = {
        0x00000FFF,
        0xFFF00000,
    },
    .kings = 0x00000000,
    .turn = BLACK,
    .moves_since_last_capture = 0,
};

bitboard_t generate_black_movers(board_t board) {
    bitboard_t not_occupied = ~(board.sides[BLACK] | board.sides[RED]);
    bitboard_t kings = board.sides[BLACK] & board.kings;
    bitboard_t movers = 0;
    movers |= (not_occupied >> 4) & board.sides[BLACK];
    movers |= ((not_occupied & LEFT_EDGE) >> 5) & board.sides[BLACK];
    movers |= ((not_occupied & RIGHT_EDGE) >> 3) & board.sides[BLACK];
    if (kings) {
        movers |= (not_occupied << 4) & kings;
        movers |= ((not_occupied & LEFT_EDGE) << 3) & kings;
        movers |= ((not_occupied & RIGHT_EDGE) << 5) & kings;
    }
    return movers;
}

bitboard_t generate_red_movers(board_t board) {
    bitboard_t not_occupied = ~(board.sides[BLACK] | board.sides[RED]);
    bitboard_t kings = board.sides[RED] & board.kings;
    bitboard_t movers = 0;
    movers |= (not_occupied << 4) & board.sides[RED];
    movers |= ((not_occupied & LEFT_EDGE) << 3) & board.sides[RED];
    movers |= ((not_occupied & RIGHT_EDGE) << 5) & board.sides[RED];
    if (kings) {
        movers |= (not_occupied >> 4) & kings;
        movers |= ((not_occupied & LEFT_EDGE) >> 5) & kings;
        movers |= ((not_occupied & RIGHT_EDGE) >> 3) & kings;
    }
    return movers;
}

bitboard_t generate_black_jumpers(board_t board) {
    bitboard_t not_occupied = ~(board.sides[BLACK] | board.sides[RED]);
    bitboard_t kings = board.sides[BLACK] & board.kings;
    bitboard_t jumpers = 0;
    bitboard_t captured = 0;
    captured = (not_occupied >> 4) & board.sides[RED];
    if (captured) jumpers |= (((captured & LEFT_EDGE) >> 5) | ((captured & RIGHT_EDGE) >> 3)) & board.sides[BLACK];
    captured = (((not_occupied & LEFT_EDGE) >> 5) | ((not_occupied & RIGHT_EDGE) >> 3)) & board.sides[RED];
    if (captured) jumpers |= (captured >> 4) & board.sides[BLACK];
    if (kings) {
        captured = (not_occupied << 4) & board.sides[RED];
        if (captured) jumpers |= (((captured & LEFT_EDGE) << 3) | ((captured & RIGHT_EDGE) << 5)) & kings;
        captured = (((not_occupied & LEFT_EDGE) << 3) | ((not_occupied & RIGHT_EDGE) << 5)) & board.sides[RED];
        if (captured) jumpers |= (captured << 4) & kings;
    }
    return jumpers;
}

bitboard_t generate_red_jumpers(board_t board) {
    bitboard_t not_occupied = ~(board.sides[BLACK] | board.sides[RED]);
    bitboard_t kings = board.sides[RED] & board.kings;
    bitboard_t jumpers = 0;
    bitboard_t captured = 0;
    captured = (not_occupied << 4) & board.sides[BLACK];
    if (captured) jumpers |= (((captured & LEFT_EDGE) << 3) | ((captured & RIGHT_EDGE) << 5)) & board.sides[RED];
    captured = (((not_occupied & LEFT_EDGE) << 3) | ((not_occupied & RIGHT_EDGE) << 5)) & board.sides[BLACK];
    if (captured) jumpers |= (captured << 4) & board.sides[RED];
    if (kings) {
        captured = (not_occupied >> 4) & board.sides[BLACK];
        if (captured) jumpers |= (((captured & LEFT_EDGE) >> 5) | ((captured & RIGHT_EDGE) >> 3)) & kings;
        captured = (((not_occupied & LEFT_EDGE) >> 5) | ((not_occupied & RIGHT_EDGE) >> 3)) & board.sides[BLACK];
        if (captured) jumpers |= (captured >> 4) & kings;
    }
    return jumpers;
}

int generate_legal_moves(board_t board, move_t * moves) {
    uint32_t num_moves = 0;
    
    bitboard_t not_occupied = ~(board.sides[BLACK] | board.sides[RED]);
    if (board.turn == BLACK) {
        bitboard_t kings = board.sides[BLACK] & board.kings;
        bitboard_t jumpers = generate_black_jumpers(board);
        if (jumpers) {
            while (jumpers) {
                bitboard_t jumper = jumpers & -jumpers;
                if ((((jumper << 4) & LEFT_EDGE & board.sides[RED]) << 3) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper << 7;
                    moves->captured = jumper << 4;
                    moves++;
                    num_moves++;
                }
                if ((((jumper << 4) & RIGHT_EDGE & board.sides[RED]) << 5) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper << 9;
                    moves->captured = jumper << 4;
                    moves++;
                    num_moves++;
                }
                if (((((jumper & LEFT_EDGE) << 3) & board.sides[RED]) << 4) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper << 7;
                    moves->captured = jumper << 3;
                    moves++;
                    num_moves++;
                }
                if (((((jumper & RIGHT_EDGE) << 5) & board.sides[RED]) << 4) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper << 9;
                    moves->captured = jumper << 5;
                    moves++;
                    num_moves++;
                }
                if (jumper & kings) {
                    if ((((jumper >> 4) & LEFT_EDGE & board.sides[RED]) >> 5) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper >> 9;
                        moves->captured = jumper >> 4;
                        moves++;
                        num_moves++;
                    }
                    if ((((jumper >> 4) & RIGHT_EDGE & board.sides[RED]) >> 3) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper >> 7;
                        moves->captured = jumper >> 4;
                        moves++;
                        num_moves++;
                    }
                    if (((((jumper & LEFT_EDGE) >> 5) & board.sides[RED]) >> 4) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper >> 9;
                        moves->captured = jumper >> 5;
                        moves++;
                        num_moves++;
                    }
                    if (((((jumper & RIGHT_EDGE) >> 3) & board.sides[RED]) >> 4) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper >> 7;
                        moves->captured = jumper >> 3;
                        moves++;
                        num_moves++;
                    }
                }
                jumpers &= ~jumper;
            }
        } else {
            bitboard_t movers = generate_black_movers(board);
            while (movers) {
                bitboard_t mover = movers & -movers;
                if ((mover << 4) & not_occupied) {
                    moves->from = mover;
                    moves->to = mover << 4;
                    moves->captured = 0;
                    moves++;
                    num_moves++;
                }
                if (((mover & LEFT_EDGE) << 3) & not_occupied) {
                    moves->from = mover;
                    moves->to = mover << 3;
                    moves->captured = 0;
                    moves++;
                    num_moves++;
                }
                if (((mover & RIGHT_EDGE) << 5) & not_occupied) {
                    moves->from = mover;
                    moves->to = mover << 5;
                    moves->captured = 0;
                    moves++;
                    num_moves++;
                }
                if (mover & kings) {
                    if ((mover >> 4) & not_occupied) {
                        moves->from = mover;
                        moves->to = mover >> 4;
                        moves->captured = 0;
                        moves++;
                        num_moves++;
                    }
                    if (((mover & LEFT_EDGE) >> 5) & not_occupied) {
                        moves->from = mover;
                        moves->to = mover >> 5;
                        moves->captured = 0;
                        moves++;
                        num_moves++;
                    }
                    if (((mover & RIGHT_EDGE) >> 3) & not_occupied) {
                        moves->from = mover;
                        moves->to = mover >> 3;
                        moves->captured = 0;
                        moves++;
                        num_moves++;
                    }
                }
                movers &= ~mover;
            }
        }
    } else {
        bitboard_t kings = board.sides[RED] & board.kings;
        bitboard_t jumpers = generate_red_jumpers(board);
        if (jumpers) {
            while (jumpers) {
                bitboard_t jumper = jumpers & -jumpers;
                if ((((jumper >> 4) & LEFT_EDGE & board.sides[BLACK]) >> 5) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper >> 9;
                    moves->captured = jumper >> 4;
                    moves++;
                    num_moves++;
                }
                if ((((jumper >> 4) & RIGHT_EDGE & board.sides[BLACK]) >> 3) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper >> 7;
                    moves->captured = jumper >> 4;
                    moves++;
                    num_moves++;
                }
                if (((((jumper & LEFT_EDGE) >> 5) & board.sides[BLACK]) >> 4) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper >> 9;
                    moves->captured = jumper >> 5;
                    moves++;
                    num_moves++;
                }
                if (((((jumper & RIGHT_EDGE) >> 3) & board.sides[BLACK]) >> 4) & not_occupied) {
                    moves->from = jumper;
                    moves->to = jumper >> 7;
                    moves->captured = jumper >> 3;
                    moves++;
                    num_moves++;
                }
                if (jumper & kings) {
                    if ((((jumper << 4) & LEFT_EDGE & board.sides[BLACK]) << 3) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper << 7;
                        moves->captured = jumper << 4;
                        moves++;
                        num_moves++;
                    }
                    if ((((jumper << 4) & RIGHT_EDGE & board.sides[BLACK]) << 5) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper << 9;
                        moves->captured = jumper << 4;
                        moves++;
                        num_moves++;
                    }
                    if (((((jumper & LEFT_EDGE) << 3) & board.sides[BLACK]) << 4) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper << 7;
                        moves->captured = jumper << 3;
                        moves++;
                        num_moves++;
                    }
                    if (((((jumper & RIGHT_EDGE) << 5) & board.sides[BLACK]) << 4) & not_occupied) {
                        moves->from = jumper;
                        moves->to = jumper << 9;
                        moves->captured = jumper << 5;
                        moves++;
                        num_moves++;
                    }
                }
                jumpers &= ~jumper;
            }
        } else {
            bitboard_t movers = generate_red_movers(board);
            while (movers) {
                bitboard_t mover = movers & -movers;
                if ((mover >> 4) & not_occupied) {
                    moves->from = mover;
                    moves->to = mover >> 4;
                    moves->captured = 0;
                    moves++;
                    num_moves++;
                }
                if (((mover & LEFT_EDGE) >> 5) & not_occupied) {
                    moves->from = mover;
                    moves->to = mover >> 5;
                    moves->captured = 0;
                    moves++;
                    num_moves++;
                }
                if (((mover & RIGHT_EDGE) >> 3) & not_occupied) {
                    moves->from = mover;
                    moves->to = mover >> 3;
                    moves->captured = 0;
                    moves++;
                    num_moves++;
                }
                if (mover & kings) {
                    if ((mover << 4) & not_occupied) {
                        moves->from = mover;
                        moves->to = mover << 4;
                        moves->captured = 0;
                        moves++;
                        num_moves++;
                    }
                    if (((mover & LEFT_EDGE) << 3) & not_occupied) {
                        moves->from = mover;
                        moves->to = mover << 3;
                        moves->captured = 0;
                        moves++;
                        num_moves++;
                    }
                    if (((mover & RIGHT_EDGE) << 5) & not_occupied) {
                        moves->from = mover;
                        moves->to = mover << 5;
                        moves->captured = 0;
                        moves++;
                        num_moves++;
                    }
                }
                movers &= ~mover;
            }
        }
    }
    
    return num_moves;
}

int game_result(board_t board) {
    if (!(generate_red_jumpers(board) | generate_red_movers(board))) {
        return 1;
    }
    if (!(generate_black_jumpers(board) | generate_black_movers(board))) {
        return -1;
    }
    return 0;
}

board_t move_piece(board_t board, move_t move) {
    board.sides[board.turn] &= ~move.from;
    board.sides[board.turn] |= move.to;
    if (board.kings & move.from) {
        board.kings &= ~move.from;
        board.kings |= move.to;
    }
    if (move.captured) {
        board.sides[!board.turn] &= ~move.captured;
        board.kings &= ~move.captured;
        board.moves_since_last_capture = -1;
    }
    if (!(board.kings & move.to) && ((board.turn == BLACK && move.to & BACK_ROW) || (board.turn == RED && move.to & FRONT_ROW))) {
        board.kings |= move.to;
        board.turn = !board.turn;
    } else if (!move.captured || (board.turn == BLACK && !(move.to & generate_black_jumpers(board))) || (board.turn == RED && !(move.to & generate_red_jumpers(board)))) {
        board.turn = !board.turn;
    }
    board.moves_since_last_capture++;
    if (board.moves_since_last_capture >= 32 || !(generate_red_jumpers(board) || generate_red_movers(board)) || !(generate_black_jumpers(board) || generate_black_movers(board))) {
        board.game_over = 1;
        board.result = game_result(board);
    }
    
    return board;
}
