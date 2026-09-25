/*
 * xrick/e_sbonus.h
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

#ifndef _E_SBONUS_H
#define _E_SBONUS_H

#include "xrick/system/basic_types.h"

/*
 * Special bonus: invisible start and stop triggers. Reaching the stop
 * trigger soon after the start trigger earns a bonus that decreases
 * with the time taken.
 */
extern bool e_sbonus_counting;  /* true between start and stop */
extern U8 e_sbonus_counter;     /* frames until the bonus decreases */
extern U16 e_sbonus_bonus;      /* bonus earned when reaching the stop */

extern void e_sbonus_start(U8);  /* start trigger entity action */
extern void e_sbonus_stop(U8);   /* stop trigger entity action */

#endif /* ndef _E_SBONUS_H */

/* eof */
