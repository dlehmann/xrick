/*
 * xrick/control.c
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
 * Control status shared by the system layer, which sets it from keyboard
 * and joystick events, and the game, which reads it.
 */

#include "xrick/control.h"

unsigned control_status = 0;  /* control_t bits of the controls held down */

/* external definitions of the inline functions, for C99 compilers */
extern inline bool control_test(control_t c);
extern inline void control_set(control_t c);
extern inline void control_clear(control_t c);

bool control_active = true;  /* false without window focus: pauses */
unsigned control_coins = 0;

/* eof */


