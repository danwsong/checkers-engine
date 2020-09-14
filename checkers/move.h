//
//  move.h
//  checkers
//
//  Created by Dan Song on 2020-06-04.
//  Copyright © 2020 daniel song. All rights reserved.
//

#ifndef move_h
#define move_h

#include <stdint.h>

#include "bitboard.h"

typedef struct {
    bitboard_t from, to, captured;
} move_t;

#endif /* move_h */
