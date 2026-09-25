/*
 * xrick/data/tiles.h
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
 * NOTES
 *
 * A tile is 8 by 8 pixels, stored as 8 rows. On PC, a row is one U16
 * (cga encoding, two bits per pixel); on ST, a row is one U32 (four bits
 * per pixel, indexes in the game palette).
 *
 * tiles_data holds tiles_nbr_banks banks of 0x100 tiles each, and
 * draw_tilesBank selects the bank that draw_tile uses:
 *
 *  PC: bank 0   main intro, texts and status bar
 *      bank 1   map intro
 *      bank 2+  game tiles, one bank per page (see map_tilesBank)
 *  ST: bank 0   intros, texts and status bar
 *      bank 1+  game tiles, one bank per page (see map_tilesBank)
 */

#ifndef _TILES_H
#define _TILES_H

#include "xrick/system/basic_types.h"

#include "xrick/config.h"

#include <stddef.h> /* size_t */

/*
 * three special tile numbers
 */
enum {
    TILES_BULLET = 0x01,
    TILES_BOMB = 0x02,
    TILES_RICK = 0x03
};

/*
 * one single tile
 */
enum { TILES_NBR_LINES = 0x08 };

#ifdef GFXPC
typedef U16 tile_t[TILES_NBR_LINES];
#endif
#ifdef GFXST
typedef U32 tile_t[TILES_NBR_LINES];
#endif

/*
 * tiles banks (each bank is 0x100 tiles)
 */
enum { TILES_NBR_TILES = 0x100 };
extern size_t tiles_nbr_banks;  /* number of banks in tiles_data */
extern tile_t *tiles_data;      /* all banks, one after the other */

#endif /* ndef _TILES_H */

/* eof */
