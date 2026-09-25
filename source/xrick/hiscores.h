/*
 * xrick/hiscores.h
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

#ifndef _HISCORES_H
#define _HISCORES_H

#include "xrick/config.h"

#ifdef ENABLE_HISCORE_FILE

/*
 * High scores kept in a text file next to the game data, see hiscores.c
 */
#define HISCORES_FILE "highscores.txt"

extern void hiscores_load(void);  /* at startup, replaces the archive's */
extern void hiscores_save(void);  /* after a new entry */

#else

/* without ENABLE_HISCORE_FILE the high scores only live in memory */
#define hiscores_load()
#define hiscores_save()

#endif /* ENABLE_HISCORE_FILE */

#endif /* ndef _HISCORES_H */

/* eof */
