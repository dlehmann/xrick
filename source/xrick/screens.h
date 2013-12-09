/*
 * xrick/include/screens.h
 *
 * Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net). All rights reserved.
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

#include "system/basic_types.h"
#include "config.h"

#define SCREEN_TIMEOUT 4000
#define SCREEN_RUNNING 0
#define SCREEN_DONE 1
#define SCREEN_EXIT 2

typedef struct {
  U16 count;  /* number of loops */
  U16 dx, dy;  /* sprite x and y deltas */
  U16 base;  /* base for sprite numbers table */
} screen_imapsteps_t;  /* description of one step */

typedef struct {
  U32 score;
  U8 name[10];
} hiscore_t;

enum { SCREEN_NBR_IMAPSL = 22 };
extern U8 screen_imapsl[SCREEN_NBR_IMAPSL];  /* sprite lists */

enum { SCREEN_NBR_IMAPSTESPS = 23 };
extern screen_imapsteps_t screen_imapsteps[SCREEN_NBR_IMAPSTESPS];  /* map intro steps */

enum { SCREEN_NBR_IMAPSOFS = 5 };
extern U8 screen_imapsofs[SCREEN_NBR_IMAPSOFS];  /* first step for each map */

enum { SCREEN_NBR_IMAPTEXT = 5 };
extern U8 *screen_imaptext[SCREEN_NBR_IMAPTEXT];  /* map intro texts */

enum { SCREEN_NBR_HISCORES = 8 };
extern hiscore_t screen_highScores[SCREEN_NBR_HISCORES];  /* highest scores (hall of fame) */


#ifdef GFXPC
extern U8 screen_imainhoft[];  /* hall of fame title */
extern U8 screen_imainrdt[];  /*rick dangerous title */
extern U8 screen_imaincdc[];  /* core design copyright text */
extern U8 screen_congrats[];  /* congratulations */
#endif
extern U8 screen_gameovertxt[];  /* game over */
extern U8 screen_pausedtxt[];  /* paused */

extern U8 screen_xrick(void);  /* splash */
extern U8 screen_introMain(void);  /* main intro */
extern U8 screen_introMap(void);  /* map intro */
extern U8 screen_gameover(void);  /* gameover */
extern U8 screen_getname(void);  /* enter you name */
extern void screen_pause(bool);  /* pause indicator */

#endif

/* eof */
