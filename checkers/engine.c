//
//  engine.c
//  checkers
//
//  Created by Dan Song on 2020-06-05.
//  Copyright © 2020 daniel song. All rights reserved.
//

#include "engine.h"

double weights[NUM_SIDES][NUM_PIECES][NUM_SQUARES] =
{
    {
        {
            0.21418930692659552,  0.27150345451962143,  0.23154072699121697,  0.1725226057609414 ,
            0.17373275160524979,  0.1712751585743759 ,  0.20732762247040476,  0.112408299100137  ,
            0.13016885801973796,  0.1469235757330272 ,  0.11582485725218072,  0.16197228920723117,
            0.1555814273984693 ,  0.14378296691657594,  0.1315550730999607 ,  0.10170528652514241,
            0.08822690014380746,  0.0999175215713361 ,  0.14786111972999585,  0.1266893112000714 ,
            0.27304097917748804,  0.1380581056346057 ,  0.13128420815926492,  0.10386545627332947,
            0.16753839557295855,  0.12975486852559073,  0.14498280313619635,  0.16982337659126848,
            0.                 ,  0.                 ,  0.                 ,  0.                 ,
        },
        {
            0.13186975032455567,  0.3426240099115206 ,  0.21497750379168176,  0.11540035299107257,
            0.11352640876729718,  0.22724817869239178,  0.19016406562817711,  0.1825380971762301 ,
            0.18886276937278706,  0.28008189991049454,  0.2639114817273752 ,  0.12419126678508836,
            0.22916440257349713,  0.18896576293876396,  0.1665518950995084 ,  0.09213098683644196,
            0.1847799913684671 ,  0.14800310073709866,  0.2149145781112893 ,  0.15357304920194126,
            0.14728337769225974,  0.1976497682639037 ,  0.19389885168423318,  0.17511388199929642,
            0.18410381764075978,  0.17085051330799494,  0.18298196524034915,  0.14397609450907511,
            0.16386019501015497,  0.2085986203021659 ,  0.18244472174058127,  0.15525808564146465,
        },
    },
    {
        {
            -0.                 , -0.                 , -0.                 , -0.                 ,
            -0.16620198361449035, -0.16135462681888982, -0.136346311311831  , -0.15309772571217742,
            -0.11845332801297452, -0.14960508842099224, -0.17934127713869458, -0.2491228572600166 ,
            -0.15432078719347894, -0.13901743535086905, -0.0895538737294223 , -0.09532362959361777,
            -0.12057465920604771, -0.15104521698953002, -0.17200161663912353, -0.16157557888797533,
            -0.14070636221688318, -0.14420723352248635, -0.15880324650754504, -0.14640921501843715,
            -0.1504526111091111 , -0.16450248915661858, -0.1619354757720141 , -0.16839098057327706,
            -0.22094405880996576, -0.2506747406018676 , -0.24599502768239478, -0.21885783669174846,
        },
        {
            -0.21915890961335238, -0.1631563246600243 , -0.2176809491258828 , -0.2179823747060088 ,
            -0.20671916419969685, -0.18187070173904935, -0.15577740667624465, -0.21747393899157622,
            -0.16052788806825058, -0.18589123372297245, -0.19533882927469126, -0.24853538493495356,
            -0.14279895056956085, -0.2017095029279896 , -0.22429172431692   , -0.2186037143949571 ,
            -0.16106985134567717, -0.2396932381592752 , -0.22250577506532118, -0.2625738523163025 ,
            -0.2850220695586756 , -0.2870365469228107 , -0.2939161305777281 , -0.2708576729401915 ,
            -0.2708445095097444 , -0.17332782105149835, -0.22223873224418006, -0.4334608686082898 ,
            -0.606875001972851  , -0.3201150125419394 , -0.26947149679632326, -0.34195564646436283,
        },
    },
};

double bias = 0.06370830424878052;

double evaluate(board_t board) {
    double evaluation = 0.0;
    bitboard_t features[2][2] = { board.sides[BLACK] & ~board.kings, board.sides[BLACK] & board.kings, board.sides[RED] & ~board.kings, board.sides[RED] & board.kings };
    for (int side = 0; side < NUM_SIDES; side++) {
        for (int piece = 0; piece < NUM_PIECES; piece++) {
            while (features[side][piece]) {
                bitboard_t pos = features[side][piece] & -features[side][piece];
                evaluation += weights[side][piece][__builtin_ctz(pos)];
                features[side][piece] &= ~pos;
            }
        }
    }
    evaluation += bias;
    return evaluation;
}

double evaluate_move(board_t board, move_t move) {
    int from = __builtin_ctz(move.from), to = __builtin_ctz(move.to), captured = __builtin_ctz(move.captured);
    double evaluation = weights[board.turn][(board.kings & move.from) != 0][to] - weights[board.turn][(board.kings & move.from) != 0][from];
    if (!(board.kings & move.to) && ((board.turn == BLACK && move.to & BACK_ROW) || (board.turn == RED && move.to & FRONT_ROW))) {
        evaluation += weights[board.turn][KING][to] - weights[board.turn][MAN][to];
    }
    if (move.captured) {
        evaluation -= weights[!board.turn][(board.kings & move.captured) != 0][captured];
    }
    return evaluation;
}

#include <stdio.h>

int nodes = 0;

double minimax(board_t board, int depth, double alpha, double beta, move_t * move) {
    move_t moves[48];
    double value = 0.0;
    move_t best_move = { .from = 0, .to = 0, .captured = 0 };
    if (board.game_over) {
        value = (board.result * (INFINITY - (DEPTH - depth)));
        nodes += 1;
    } else if (depth == 0) {
        value = evaluate(board);
        nodes += 1;
    } else if (board.turn == BLACK) {
        value = -INFINITY;
        int num_moves = generate_legal_moves(board, moves);
        for (int i = 0; i < num_moves; i++) {
//            if (depth == DEPTH) {
//                printf("%d\n", i);
//            }
            double minimax_value = minimax(move_piece(board, moves[i]), depth - 1, alpha, beta, move);
            if (minimax_value > value) {
                value = minimax_value;
                best_move = moves[i];
            }
            if (value > alpha)
                alpha = value;
            if (alpha > beta)
                break;
        }
    } else {
        value = INFINITY;
        int num_moves = generate_legal_moves(board, moves);
        for (int i = 0; i < num_moves; i++) {
//            if (depth == DEPTH) {
//                printf("%d\n", i);
//            }
            double minimax_value = minimax(move_piece(board, moves[i]), depth - 1, alpha, beta, move);
            if (minimax_value < value) {
                value = minimax_value;
                best_move = moves[i];
            }
            if (value < beta)
                beta = value;
            if (beta < alpha)
                break;
        }
    }
    *move = best_move;
//    if (depth == DEPTH) {
//        printf("%d positions\n", nodes);
//    }
    return value;
}
