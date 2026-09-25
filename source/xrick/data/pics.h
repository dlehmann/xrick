/*
 * xrick/data/pics.h
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

#ifndef _PICS_H
#define _PICS_H

#include "xrick/config.h"

#ifdef GFXST

#include "xrick/system/basic_types.h"

/*
 * A picture in the Atari ST format: each U32 holds 8 pixels of 4 bits,
 * indexes in the game palette
 */
typedef struct {
  U16 width;     /* size in pixels */
  U16 height;
  U16 xPos;      /* position on the screen */
  U16 yPos;
  U32 *pixels;   /* width / 8 * height U32, row by row */
} pic_t;

extern pic_t *pic_haf;       /* hall of fame title */
extern pic_t *pic_congrats;  /* congratulations, on the name entry screen */
extern pic_t *pic_splash;    /* Rick Dangerous title screen */

#endif /* GFXST */

#endif /* ndef _PICS_H */

/* eof */


