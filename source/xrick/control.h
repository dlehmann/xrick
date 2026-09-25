/*
 * xrick/control.h
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

#ifndef _CONTROL_H
#define _CONTROL_H

#include "xrick/system/basic_types.h"

/*
 * Controls, one bit each in control_status
 */
typedef enum
{
    Control_UP = (1 << 0),
    Control_DOWN = (1 << 1),
    Control_LEFT = (1 << 2),
    Control_RIGHT = (1 << 3),
    Control_PAUSE = (1 << 4),
    Control_END = (1 << 5),
    Control_EXIT = (1 << 6),
    Control_FIRE = (1 << 7),
    Control_B = (1 << 8),  /* only used by the konami code */
    Control_A = (1 << 9)
} control_t;

extern unsigned control_status;  /* controls held down, control_t bits */

/* test if a control is held down */
inline bool control_test(control_t c) { return control_status & c; }
/* mark a control as held down */
inline void control_set(control_t c) { control_status |= c; }
/* mark a control as released */
inline void control_clear(control_t c) { control_status &= ~c; }

extern bool control_active;  /* false without window focus (ENABLE_FOCUS) */
extern unsigned control_coins;  /* coins inserted and not used yet */

#endif /* ndef _CONTROL_H */

/* eof */


