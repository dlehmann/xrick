/*
 * xrick/system/syskbd_sdl.c
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

#include <SDL.h>

#include "xrick/system/system.h"

/*
 * Global variables: key codes of the controls, SDL version. The arrow
 * keys work too, see sysevt_sdl.c; --keys changes the first five.
 */
U8 syskbd_up = SDLK_o;
U8 syskbd_down = SDLK_k;
U8 syskbd_left = SDLK_z;
U8 syskbd_right = SDLK_x;
U8 syskbd_pause = SDLK_p;
U8 syskbd_end = SDLK_e;
U8 syskbd_xtra = SDLK_ESCAPE;
U8 syskbd_fire = SDLK_SPACE;
U8 syskbd_coin = SDLK_c;
U8 syskbd_b = SDLK_b;
U8 syskbd_a = SDLK_a;

/* eof */


