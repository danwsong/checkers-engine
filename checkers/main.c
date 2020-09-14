//
//  main.c
//  checkers
//
//  Created by Dan Song on 2020-06-04.
//  Copyright © 2020 daniel song. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "engine.h"

void print_board(board_t board) {
    for (uint32_t i = 0x1F; i < 0x20; i--) {
        if (i & 0x4) {
            printf("   ");
        }
        if (board.sides[BLACK] & (1 << i)) {
            if (board.kings & (1 << i)) {
                printf(" B ");
            } else {
                printf(" b ");
            }
        } else if (board.sides[RED] & (1 << i)) {
            if (board.kings & (1 << i)) {
                printf(" R ");
            } else {
                printf(" r ");
            }
        } else {
            printf(" _ ");
        }
        if (!(i & 0x4)) {
            printf("   ");
        }
        if (!(i & 0x3)) {
            printf("\n");
        }
    }
    if (board.turn == BLACK) {
        printf("Black to move...\n");
    } else {
        printf("White to move...\n");
    }
}

#define COMPUTER_SIDE   RED

int main(int argc, const char * argv[]) {
    board_t cur_board;
    cur_board = starting_board;
    print_board(cur_board);
    while (!cur_board.game_over) {
        move_t move = { .from = 0, .to = 0, .captured = 0 };
        if (cur_board.turn == COMPUTER_SIDE) {
//            clock_t start = clock();
            double minimax_evaluation = minimax(cur_board, DEPTH, -INFINITY, INFINITY, &move);
//            printf("%lf seconds...\n", (double) (clock() - start) / (double) CLOCKS_PER_SEC);
            printf("Current evaluation: %lf\n", minimax_evaluation);
            printf("%02d-%02d\n", __builtin_ctz(move.from) + 1, __builtin_ctz(move.to) + 1);
        } else {
            int from = 0, to = 0, captured = 0;
            scanf("%02d-%02d-%02d", &from, &to, &captured);
            move.from = 1 << (from - 1);
            move.to = 1 << (to - 1);
            if (captured) {
                move.captured = 1 << (captured - 1);
            }
        }
        cur_board = move_piece(cur_board, move);
        print_board(cur_board);
    }
    
    return 0;
}

//printf("%02d: From %02d to %02d", i, __builtin_ctz(moves[i].from), __builtin_ctz(moves[i].to));
//if (moves[i].captured) {
//    printf(" capturing %02d", __builtin_ctz(moves[i].captured));
//}
//printf("\n");

//            for (int i = 0; i < 32; i++) fprintf(features_csv, "%d,", ((cur_board.sides[BLACK] & ~cur_board.kings) >> i) & 1);
//            for (int i = 0; i < 32; i++) fprintf(features_csv, "%d,", ((cur_board.sides[BLACK] & cur_board.kings) >> i) & 1);
//            for (int i = 0; i < 32; i++) fprintf(features_csv, "%d,", ((cur_board.sides[RED] & ~cur_board.kings) >> i) & 1);
//            for (int i = 0; i < 32; i++) fprintf(features_csv, "%d,", ((cur_board.sides[RED] & cur_board.kings) >> i) & 1);
//            fprintf(features_csv, "\n");

//        printf("%d moves:\n", num_moves);
//        for (int i = 0; i < num_moves; i++) {
//            printf("%02d: From %02d to %02d", i, __builtin_ctz(moves[i].from), __builtin_ctz(moves[i].to));
//            if (moves[i].captured) {
//                printf(" capturing %02d", __builtin_ctz(moves[i].captured));
//            }
//            printf("\n");
//        }
//        uint32_t index;
//        scanf("%02d", &index);
