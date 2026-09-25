/*
 * xrick/scroller.h
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

#ifndef _SCROLLER_H
#define _SCROLLER_H

/*
 * Scrolling the map one screen part (8 tile rows) up or down, one tile
 * row per frame. scroll_up and scroll_down are called once per frame and
 * return one of these codes.
 */
#define SCROLL_RUNNING 1  /* call again next frame */
#define SCROLL_DONE 0     /* scrolling is over */

#define SCROLL_PERIOD 24  /* time between frames while scrolling, in ms */

#include "xrick/system/basic_types.h"

extern U8 scroll_up(void);    /* the map moves up: rick goes down */
extern U8 scroll_down(void);  /* the map moves down: rick goes up */

#endif /* ndef _SCROLLER_H */

/* eof */


