/*
 * xrick/e_bomb.h
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

#ifndef _E_BOMB_H
#define _E_BOMB_H

#include "xrick/system/basic_types.h"

/*
 * The stick of dynamite laid by Rick: it always uses entity slot 3
 */
#define E_BOMB_NO 3
#define E_BOMB_ENT ent_ents[E_BOMB_NO]
#define E_BOMB_TICKER (0x2D)  /* frames from laying to the end of the blast */

extern bool e_bomb_lethal;  /* true while exploding */
extern U8 e_bomb_ticker;    /* frames left, counting down */
extern U8 e_bomb_xc;        /* center of the explosion (pixels, map) */
extern U16 e_bomb_yc;

extern bool e_bomb_hit(U8);  /* is entity e within the blast? */
extern void e_bomb_init(U16, U16);  /* lay a bomb at x, y */
extern void e_bomb_action(U8);

#endif /* ndef _E_BOMB_H */

/* eof */
