/*
 * xrick/screens.h
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

#ifndef _SCREENS_H
#define _SCREENS_H

#include "xrick/system/basic_types.h"
#include "xrick/config.h"

#include <stddef.h> /* size_t */

/*
 * Screens: the splash screen, intros, level select, game over, name entry
 * and pause. Each screen function is called once per frame, draws what
 * changed and returns one of these codes.
 */
#define SCREEN_TIMEOUT 4000  /* time a screen shows without input, in ms */
#define SCREEN_RUNNING 0     /* call again next frame */
#define SCREEN_DONE 1        /* the screen is over */
#define SCREEN_EXIT 2        /* exit requested */
#define SCREEN_SELECT 3  /* level select requested (screen_introMain) */
#define SCREEN_BACK 4    /* back to the main intro (screen_selectMap) */

typedef struct {
  U16 count;  /* number of loops */
  U16 dx, dy;  /* sprite x and y deltas */
  U16 base;  /* base for sprite numbers table */
} screen_imapsteps_t;  /* description of one step */

/*
 * One entry of the hall of fame
 */
enum { HISCORE_NAME_SIZE = 10 };
typedef struct {
  U32 score;
  U8 name[HISCORE_NAME_SIZE];  /* tiles, '@' for blanks, not terminated */
} hiscore_t;

/* from the data archive, see resources.c; texts are tiles lists */
extern size_t screen_nbr_imapsl;
extern U8 *screen_imapsl;  /* sprite lists */

extern size_t screen_nbr_imapstesps;
extern screen_imapsteps_t *screen_imapsteps;  /* map intro steps */

extern size_t screen_nbr_imapsofs;
extern U8 *screen_imapsofs;  /* first step for each map */

extern size_t screen_nbr_imaptext;
extern U8 **screen_imaptext;  /* map intro texts */

extern size_t screen_nbr_hiscores;
extern hiscore_t *screen_highScores;  /* highest scores (hall of fame) */

#ifdef GFXPC
extern U8 *screen_imainhoft;  /* hall of fame title */
extern U8 *screen_imainrdt;  /* rick dangerous title */
extern U8 *screen_imaincdc;  /* core design copyright text */
extern U8 *screen_congrats;  /* congratulations */
#endif /* GFXPC */
extern U8 *screen_gameovertxt;  /* game over */
extern U8 *screen_pausedtxt;  /* paused */

extern U8 screen_xrick(void);  /* splash */
extern U8 screen_introMain(void);  /* main intro */
extern U8 screen_introMap(void);  /* map intro */
extern U8 screen_selectMap(void);  /* level select */
extern U8 screen_gameover(void);  /* gameover */
extern U8 screen_getname(void);  /* enter you name */
extern void screen_pause(bool);  /* pause indicator */

#endif /* ndef _SCREENS_H */

/* eof */
