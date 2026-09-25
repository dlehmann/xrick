/*
 * xrick/scroller.c
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
 * Scrolling: when rick gets near the top or the bottom of the screen,
 * the map scrolls by 8 tile rows, one row per frame. Entities move with
 * the map; those of the rows scrolling into view are created at the end.
 */

#include "xrick/scroller.h"

#include "xrick/game.h"
#include "xrick/debug.h"
#include "xrick/draw.h"
#include "xrick/maps.h"
#include "xrick/ents.h"

/*
 * Local variables
 */
static U8 period;  /* game period to restore when done */

/*
 * Scroll up: shift map_map up by one row, and entities with it
 *
 * return: SCROLL_RUNNING, SCROLL_DONE
 */
U8
scroll_up(void)
{
  U8 i, j;
  static U8 n = 0;

  /* last call: restore */
  if (n == 8) {
    n = 0;
    game_period = period;
    return SCROLL_DONE;
  }

  /* first call: prepare */
  if (n == 0) {
    period = game_period;
    game_period = SCROLL_PERIOD;
  }

  /* translate map */
  for (i = MAP_ROW_SCRTOP; i < MAP_ROW_HBBOT; i++)
    for (j = 0x00; j < 0x20; j++)
      map_map[i][j] = map_map[i + 1][j];

  /* translate entities */
  for (i = 0; ent_ents[i].n != 0xFF; i++) {
    if (ent_ents[i].n) {
      ent_ents[i].ysave -= 8;
      ent_ents[i].trig_y -= 8;
      ent_ents[i].y -= 8;
      if (ent_ents[i].y & 0x8000) {  /* map coord. from 0x0000 to 0x0140 */
    IFDEBUG_SCROLLER(
      sys_printf("xrick/scroller: entity %#04X is gone\n", i);
      );
    ent_ents[i].n = 0;
      }
    }
  }

  /* display */
  draw_map();
  ent_draw();
  draw_drawStatus();
  map_frow++;

  /* loop */
  if (n++ == 7) {  /* last row: the hidden bottom is new */
    /* activate visible entities */
    ent_actvis(map_frow + MAP_ROW_HBTOP, map_frow + MAP_ROW_HBBOT);

    /* prepare map */
    map_expand();

    /* display */
    draw_map();
    ent_draw();
    draw_drawStatus();
  }

  game_rects = &draw_SCREENRECT;

  return SCROLL_RUNNING;
}

/*
 * Scroll down: shift map_map down by one row, and entities with it
 *
 * return: SCROLL_RUNNING, SCROLL_DONE
 */
U8
scroll_down(void)
{
  U8 i, j;
  static U8 n = 0;

  /* last call: restore */
  if (n == 8) {
    n = 0;
    game_period = period;
    return SCROLL_DONE;
  }

  /* first call: prepare */
  if (n == 0) {
    period = game_period;
    game_period = SCROLL_PERIOD;
  }

  /* translate map */
  for (i = MAP_ROW_SCRBOT; i > MAP_ROW_HTTOP; i--)
    for (j = 0x00; j < 0x20; j++)
      map_map[i][j] = map_map[i - 1][j];

  /* translate entities */
  for (i = 0; ent_ents[i].n != 0xFF; i++) {
    if (ent_ents[i].n) {
      ent_ents[i].ysave += 8;
      ent_ents[i].trig_y += 8;
      ent_ents[i].y += 8;
      if (ent_ents[i].y > 0x0140) {  /* map coord. from 0x0000 to 0x0140 */
    IFDEBUG_SCROLLER(
      sys_printf("xrick/scroller: entity %#04X is gone\n", i);
      );
    ent_ents[i].n = 0;
      }
    }
  }

  /* display */
  draw_map();
  ent_draw();
  draw_drawStatus();
  map_frow--;

  /* loop */
  if (n++ == 7) {  /* last row: the hidden top is new */
    /* activate visible entities */
    ent_actvis(map_frow + MAP_ROW_HTTOP, map_frow + MAP_ROW_HTBOT);

    /* prepare map */
    map_expand();

    /* display */
    draw_map();
    ent_draw();
    draw_drawStatus();
  }

  game_rects = &draw_SCREENRECT;

  return SCROLL_RUNNING;
}

/* eof */
