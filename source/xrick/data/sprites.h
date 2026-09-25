/*
 * xrick/data/sprites.h
 *
 * Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net).
 * Copyright (C) 2008-2014 Pierluigi Vicinanza.
 * All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

/*
 * NOTES -- PC version
 *
 * A sprite consists in 4 columns and 21 rows of (U16 mask, U16 pict),
 * each pair representing 8 pixels (cga encoding, two bits per pixels).
 * draw_sprite2 shifts them to any pixel position while drawing.
 *
 * NOTES -- ST version
 *
 * A sprite consists in 4 columns and 21 rows of U32, each representing
 * 8 pixels of 4 bits (indexes in the game palette); color 0 is
 * transparent.
 *
 * Sprites are loaded from the data archive by resources.c.
 */

#ifndef _SPRITES_H_
#define _SPRITES_H_

#include "xrick/config.h"

#include "xrick/system/basic_types.h"

#include <stddef.h> /* size_t */

#ifdef GFXPC

/* 8 pixels of a sprite */
typedef struct {
  U16 mask;  /* bits of the background to keep */
  U16 pict;  /* bits of the sprite to draw over it */
} spriteX_t;

enum {
    SPRITES_NBR_ROWS = 21,
    SPRITES_NBR_COLS = 4
};
typedef spriteX_t sprite_t[SPRITES_NBR_COLS][SPRITES_NBR_ROWS];   /* one sprite */

#endif /* GFXPC */


#ifdef GFXST

enum {
    SPRITES_NBR_ROWS = 21,
    SPRITES_NBR_COLS = 4,
    SPRITES_NBR_DATA = SPRITES_NBR_ROWS * SPRITES_NBR_COLS
};
typedef U32 sprite_t[SPRITES_NBR_DATA];

#endif /* GFXST */

extern size_t sprites_nbr_sprites;  /* number of sprites in sprites_data */
extern sprite_t *sprites_data;      /* all sprites */

#endif /* ndef _SPRITES_H_ */

/* eof */

