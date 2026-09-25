/*
 * xrick/data/tiles.c
 *
 * Copyright (C) 2008-2014 Pierluigi Vicinanza. All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#include "xrick/data/tiles.h"

/*
 * Tiles, loaded from the data archive by resources.c
 */

/*
 * globals
 */
size_t tiles_nbr_banks = 0;  /* number of banks in tiles_data */
tile_t *tiles_data = NULL;   /* all banks, one after the other */

/* eof */
