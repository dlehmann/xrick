/*
 * xrick/scr_imap.c
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

#include "xrick/screens.h"
#include "xrick/lang.h"

#include "xrick/game.h"
#include "xrick/rects.h"
#include "xrick/draw.h"
#include "xrick/control.h"
#include "xrick/maps.h"
#include "xrick/system/system.h"

#include <string.h>

/*
 * local vars
 */
static U16 step;              /* current step */
static U16 count;             /* number of loops for current step */
static U16 run;               /* 1 = run, 0 = no more step */
static U8 flipflop;           /* flipflop for top, bottom, left, right */
static U8 spnum;              /* sprite number */
static U16 spx, spdx;         /* sprite x position and delta */
static U16 spy, spdy;         /* sprite y position and delta */
static U16 spbase, spoffs;    /* base, offset for sprite numbers table */
static U8 seq = 0;            /* anim sequence */

static rect_t anim_rect = { 128, 16 + 16, 64, 64, NULL }; /* anim rectangle */

/*
 * prototypes
 */
static void drawtb(void);
static void drawlr(void);
static void drawsprite(void);
static void drawcenter(void);
static void nextstep(void);
static void anim(void);
static void init(void);

/*
 * Map introduction
 *
 * ASM: 1948
 *
 * return: SCREEN_RUNNING, SCREEN_DONE, SCREEN_EXIT
 */
U8
screen_introMap(void)
{
  switch (seq) {
  case 0:
    sysvid_clear();

#ifdef GFXPC
    draw_tilesBank = 1;
    draw_filter = 0xAAAA;
#endif
#ifdef GFXST
    draw_tilesBank = 0;
#endif
    draw_tllst = LANG_TEXT(lang_imaptext[game_map], screen_imaptext[game_map]);
    draw_setfb(40, 16);
    draw_tilesSubList();

    draw_setfb(40, 104);
#ifdef GFXPC
    draw_filter = 0x5555;
#endif
    draw_tilesList();

    game_rects = NULL;

#ifdef GFXPC
    draw_filter = 0xFFFF;
#endif

    init();
    nextstep();
    drawcenter();
    drawtb();
    drawlr();
    drawsprite();
    draw_drawStatus();

    game_rects = &draw_SCREENRECT;

#ifdef ENABLE_SOUND
    game_setmusic(map_maps[game_map].tune, 1);
#endif

    seq = 1;
    break;
  case 1:  /* top and bottom borders */
    drawtb();
    game_rects = &anim_rect;
    seq = 2;
    break;
  case 2:  /* background and sprite */
    anim();
    drawcenter();
    drawsprite();
    game_rects = &anim_rect;
    seq = 3;
    break;
  case 3:  /* all borders */
    drawtb();
    drawlr();
    game_rects = &anim_rect;
    seq = 1;
    break;
  case 4:  /* wait for key release */
    if (!(control_test(Control_FIRE)))
      seq = 5;
    break;
  }

  if (control_test(Control_FIRE)) {  /* end as soon as key pressed */
    seq = 4;
  }

  if (control_test(Control_EXIT))  /* check for exit request */
    return SCREEN_EXIT;

  if (seq == 5) {  /* end as soon as key pressed */
    sysvid_clear();
    seq = 0;
    return SCREEN_DONE;
  }
  else
    return SCREEN_RUNNING;
}


/*
 * Level select, reached with the konami code on the main intro
 */
#define SELECT_NBR_MAPS 4
#define SELECT_TIMEOUT 10000  /* back to the main intro, in ms */
#define SELECT_BLINK 500      /* insert coin blink half period, in ms */
#define SELECT_TITLE_Y 16
#define SELECT_NUMBER_Y 120
#define SELECT_PROMPT_Y 152
#define TILE_ARROW '\074'     /* left arrow, mirrored for the right one */

static U8 *select_titles[SELECT_NBR_MAPS] = {
  (U8 *)"SOUTH@AMERICA@1945\376", (U8 *)"EGYPT\376",
  (U8 *)"EUROPE,@CASTLE\376", (U8 *)"EUROPE,@MISSILE@BASE\376"
};

/*
 * Length of a tiles list ending with 0xfe
 */
static U8
tiles_len(const U8 *tiles)
{
  U8 n;

  for (n = 0; tiles[n] != 0xfe; n++);
  return n;
}

/*
 * Draw a tiles list centered on the screen, over a cleared line
 */
static void
drawcentered(U8 *tiles, U16 y)
{
  memset(sysvid_fb + y * SYSVID_WIDTH, 0, 8 * SYSVID_WIDTH);
  if (!tiles)
    return;
  draw_setfb(SYSVID_WIDTH / 2 - tiles_len(tiles) * 4, y);
  draw_tllst = tiles;
  draw_tilesSubList();
}

/*
 * Draw the prompt: insert coin (blinking) in coin mode without coins,
 * else press fire
 */
static void
select_drawPrompt(void)
{
  if (sysarg_args_coins && control_coins == 0)
    drawcentered((sys_gettime() / SELECT_BLINK) & 1 ? NULL :
      LANG_TEXT(lang_insertcointxt, (U8 *)"INSERT@COIN\376"),
      SELECT_PROMPT_Y);
  else
    drawcentered(LANG_TEXT(lang_pressfiretxt,
      (U8 *)"PRESS@FIRE@TO@START\376"), SELECT_PROMPT_Y);
}

/*
 * Draw the level select screen for game_map
 */
static void
select_draw(void)
{
  U8 *title, *number;
  U8 s[32];
  U8 i, n, y, t;
  U16 x;
  U8 *f;
  char digit = '1' + game_map;

  sysvid_clear();
#ifdef GFXPC
  draw_tilesBank = 1;
  draw_filter = 0xAAAA;
#endif
#ifdef GFXST
  draw_tilesBank = 0;
#endif

  /* title between arrows */
  title = LANG_TEXT(lang_worldtxt[game_map], select_titles[game_map]);
  n = tiles_len(title);
  x = SYSVID_WIDTH / 2 - (n + 4) * 4;
  draw_setfb(x, SELECT_TITLE_Y);
  draw_tile(TILE_ARROW);
  draw_setfb(x + 16, SELECT_TITLE_Y);
  draw_tllst = title;
  draw_tilesSubList();
  x += (n + 3) * 8;
  draw_setfb(x, SELECT_TITLE_Y);
  draw_tile(TILE_ARROW);
  for (y = 0; y < 8; y++) {  /* mirror */
    f = sysvid_fb + (SELECT_TITLE_Y + y) * SYSVID_WIDTH + x;
    for (i = 0; i < 4; i++) {
      t = f[i]; f[i] = f[7 - i]; f[7 - i] = t;
    }
  }

  /* world number, '#' stands for the world then the number of worlds */
  number = LANG_TEXT(lang_worldnumbertxt, (U8 *)"WORLD@#@OF@#\376");
  for (i = 0; number[i] != 0xfe && i < sizeof(s) - 1; i++) {
    s[i] = number[i];
    if (s[i] == '#') {
      s[i] = digit;
      digit = '0' + SELECT_NBR_MAPS;
    }
  }
  s[i] = 0xfe;
#ifdef GFXPC
  draw_filter = 0x5555;
#endif
  drawcentered(s, SELECT_NUMBER_Y);
  select_drawPrompt();
#ifdef GFXPC
  draw_filter = 0xFFFF;
#endif

  /* picture */
  init();
  nextstep();
  drawcenter();
  drawtb();
  drawlr();
  drawsprite();
}

/*
 * Level select
 *
 * return: SCREEN_RUNNING, SCREEN_DONE (start game_map), SCREEN_BACK,
 *         SCREEN_EXIT
 */
U8
screen_selectMap(void)
{
  static U8 sel = 0;
  static U8 map0;             /* map to restore when going back */
  static unsigned prev;       /* controls of the previous frame */
  static unsigned coins_seen; /* coins when the screen was entered */
  static U32 tm;
  unsigned pressed;

  if (sel == 0) {
    map0 = game_map;
    prev = control_status;  /* keys held when entering are no presses */
    coins_seen = control_coins;
    tm = sys_gettime();
    select_draw();
    game_rects = &draw_SCREENRECT;
    sel = 1;
    return SCREEN_RUNNING;
  }

  pressed = control_status & ~prev;
  prev = control_status;

  if (sel == 4) {  /* wait for key released */
    if (!(control_test(Control_FIRE)))
      sel = 5;
  }
  else if (pressed & (Control_LEFT | Control_RIGHT)) {
    game_map = (game_map + ((pressed & Control_RIGHT) ? 1 : SELECT_NBR_MAPS - 1))
      % SELECT_NBR_MAPS;
    select_draw();
    tm = sys_gettime();
  }
  else {
    switch (sel) {  /* animate the picture, as in the map intro */
    case 1:
      drawtb();
      sel = 2;
      break;
    case 2:
      anim();
      drawcenter();
      drawsprite();
      sel = 3;
      break;
    case 3:
      drawtb();
      drawlr();
      sel = 1;
      break;
    }
    select_drawPrompt();

    /* start as on the main intro: coin inserted, or fire with coins */
    if (sysarg_args_coins && control_coins > coins_seen)
      sel = 5;
    else if ((pressed & Control_FIRE) &&
             (!sysarg_args_coins || control_coins > 0))
      sel = 4;
  }

  if (control_test(Control_EXIT))  /* check for exit request */
    return SCREEN_EXIT;

  if (sel == 5) {  /* start the selected map */
    if (sysarg_args_coins)
      control_coins--;  /* one coin per game */
    sysvid_clear();
    sel = 0;
    return SCREEN_DONE;
  }

  if (sel != 4 && sys_gettime() - tm > SELECT_TIMEOUT) {
    game_map = map0;
    sysvid_clear();
    sel = 0;
    return SCREEN_BACK;
  }

  return SCREEN_RUNNING;
}


/*
 * Display top and bottom borders (0x1B1F)
 *
 */
static void
drawtb(void)
{
  U8 i;

  flipflop++;
  if (flipflop & 0x01) {
    draw_setfb(136, 16 + 16);
    for (i = 0; i < 6; i++)
      draw_tile(0x40);
    draw_setfb(136, 72 + 16);
    for (i = 0; i < 6; i++)
      draw_tile(0x06);
  }
  else {
    draw_setfb(136, 16 + 16);
    for (i = 0; i < 6; i++)
      draw_tile(0x05);
    draw_setfb(136, 72 + 16);
    for (i = 0; i < 6; i++)
      draw_tile(0x40);
  }
}


/*
 * Display left and right borders (0x1B7C)
 *
 */
static void
drawlr(void)
{
  U8 i;

  if (flipflop & 0x02) {
    for (i = 0; i < 8; i++) {
      draw_setfb(128, 16 + i * 8 + 16);
      draw_tile(0x04);
      draw_setfb(184, 16 + i * 8 + 16);
      draw_tile(0x04);
    }
  }
  else {
    for (i = 0; i < 8; i++) {
      draw_setfb(128, 16 + i * 8 + 16);
      draw_tile(0x2B);
      draw_setfb(184, 16 + i * 8 + 16);
      draw_tile(0x2B);
    }
  }
}


/*
 * Draw the sprite (0x19C6)
 *
 */
static void
drawsprite(void)
{
  draw_sprite(spnum, 136 + ((spx << 1) & 0x1C), 24 + (spy << 1) + 16);
}


/*
 * Draw the background (0x1AF1)
 *
 */
static void
drawcenter(void)
{
  static U8 tn0[] = { 0x07, 0x5B, 0x7F, 0xA3, 0xC7 };
  U8 i, j, tn;

  tn = tn0[game_map];
  for (i = 0; i < 6; i++) {
    draw_setfb(136, (24 + 8 * i) + 16);
    for (j = 0; j < 6; j++)
      draw_tile(tn++);
  }
}


/*
 * Next Step (0x1A74)
 *
 */
static void
nextstep(void)
{
  if (screen_imapsteps[step].count) {
    count = screen_imapsteps[step].count;
    spdx = screen_imapsteps[step].dx;
    spdy = screen_imapsteps[step].dy;
    spbase = screen_imapsteps[step].base;
    spoffs = 0;
    step++;
  }
  else {
    run = 0;
  }
}


/*
 * Anim (0x1AA8)
 *
 */
static void
anim(void)
{
  U8 i;

  if (run) {
    i = screen_imapsl[spbase + spoffs];
    if (i == 0) {
      spoffs = 0;
      i = screen_imapsl[spbase];
    }
    spnum = i;
    spoffs++;
    spx += spdx;
    spy += spdy;
    count--;
    if (count == 0)
      nextstep();
  }
}


/*
 * Initialize (0x1A43)
 *
 */
static void
init(void)
{
  run = 0; run--;
  step = screen_imapsofs[game_map];
  spx = screen_imapsteps[step].dx;
  spy = screen_imapsteps[step].dy;
  step++;
  spnum = 0; /* NOTE spnum in [8728] is never initialized ? */
}

/* eof */



