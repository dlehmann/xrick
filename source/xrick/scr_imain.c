/*
 * xrick/scr_imain.c
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

#include "xrick/game.h"
#include "xrick/draw.h"
#include "xrick/control.h"
#include "xrick/lang.h"
#ifdef GFXST
#include "xrick/data/pics.h"
#endif
#include "xrick/system/system.h"

#include <string.h>

/*
 * coin mode (--coins): text below the pictures, between the logos
 */
#define COINS_Y 184
#define COINS_WIDTH 16      /* in tiles, the black band of the title */
#define COINS_BLINK 500     /* insert coin blink half period, in ms */

/* picture behind the text line, to erase the text */
static U8 coins_background[8 * SYSVID_WIDTH];

/*
 * Draw a line of tiles centered on the screen, over the picture
 *
 * tiles: tiles list ending with 0xfe, NULL to erase the line only
 */
static void
coins_drawLine(U8 *tiles)
{
    U8 *line = sysvid_fb + COINS_Y * SYSVID_WIDTH;
    size_t i;

    memcpy(line, coins_background, sizeof(coins_background));
    if (!tiles)
        return;
    for (i = 0; tiles[i] != 0xfe; i++);
    draw_setfb(SYSVID_WIDTH / 2 - i * 4, COINS_Y);
    draw_tllst = tiles;
    draw_tilesSubList();

    /* tiles are opaque: let the picture show through their background */
    for (i = 0; i < sizeof(coins_background); i++)
        if (line[i] == 0)
            line[i] = coins_background[i];
}

/*
 * Draw the blinking insert coin prompt, or the number of coins if any
 *
 * newpic: the picture was just drawn
 */
static void
coins_draw(bool newpic)
{
    U8 s[COINS_WIDTH + 1];
    U8 *label = LANG_TEXT(lang_creditstxt, (U8 *)"CREDITS\376");
    size_t n;

    if (newpic)
        memcpy(coins_background, sysvid_fb + COINS_Y * SYSVID_WIDTH,
               sizeof(coins_background));
#ifdef GFXPC
    draw_filter = 0xffff;
#endif
    if (control_coins == 0) {
        coins_drawLine((sys_gettime() / COINS_BLINK) & 1 ? NULL :
            LANG_TEXT(lang_insertcointxt, (U8 *)"INSERT@COIN\376"));
        return;
    }

    for (n = 0; label[n] != 0xfe && n < COINS_WIDTH - 3; n++)
        s[n] = label[n];
    sys_snprintf((char *)s + n, sizeof(s) - n, "@%u", control_coins);
    s[strlen((char *)s)] = 0xfe;
    coins_drawLine(s);
}

/*
 * konami code: up up down down left right left right B A. It leads to
 * the level select screen.
 */
#define KONAMI_LENGTH 10
#define KONAMI_TIMEOUT 3000  /* max time between two keys, in ms */
#define KONAMI_KEYS (Control_UP | Control_DOWN | Control_LEFT | \
                     Control_RIGHT | Control_B | Control_A)

static const control_t konami_keys[KONAMI_LENGTH] = {
    Control_UP, Control_UP, Control_DOWN, Control_DOWN,
    Control_LEFT, Control_RIGHT, Control_LEFT, Control_RIGHT,
    Control_B, Control_A
};
static U8 konami_pos;         /* number of keys of the code entered */
static unsigned konami_prev;  /* controls of the previous frame */
static U32 konami_tm;         /* time of the last key */

/*
 * Follow the keys pressed
 *
 * return: true when the whole code has been entered
 */
static bool
konami_update(void)
{
    unsigned pressed = control_status & ~konami_prev & KONAMI_KEYS;

    konami_prev = control_status;
    if (!pressed) {
        if (konami_pos && sys_gettime() - konami_tm > KONAMI_TIMEOUT)
            konami_pos = 0;
        return false;
    }
    konami_tm = sys_gettime();

    if (pressed == (unsigned)konami_keys[konami_pos])
        konami_pos++;
    else if (pressed == Control_UP)
        konami_pos = (konami_pos == 2) ? 2 : 1;  /* up up up ... */
    else
        konami_pos = 0;

    if (konami_pos == KONAMI_LENGTH) {
        konami_pos = 0;
        return true;
    }
    return false;
}

/*
 * Main introduction
 *
 * return: SCREEN_RUNNING, SCREEN_DONE, SCREEN_SELECT, SCREEN_EXIT
 */
U8
screen_introMain(void)
{
    static U8 seq = 0;
    static U8 seen = 0;
    static bool first = true;
    static U8 period = 0;
    static U32 tm = 0;
    static unsigned coins_seen = 0;
    bool newpic;

    if (seq == 0) {
        coins_seen = control_coins;
        konami_pos = 0;
        konami_prev = control_status;
        draw_tilesBank = 0;
        if (first)
            seq = 1;
        else
            seq = 4;
        period = game_period;
        game_period = 50;
        game_rects = &draw_SCREENRECT;
#ifdef ENABLE_SOUND
        game_setmusic(soundTune5, -1);
#endif
    }

    newpic = (seq == 1 || seq == 4);  /* title or hall of fame drawn now */

    if (konami_update()) {
#ifdef ENABLE_SOUND
        syssnd_play(soundBonus, 1);
#endif
        seq = 9;
    }

    switch (seq)
    {
        case 1:  /* display Rick Dangerous title and Core Design copyright */
        {
            sysvid_clear();
            tm = sys_gettime();
#ifdef GFXPC
            /* Rick Dangerous title */
            draw_tllst = (U8 *)screen_imainrdt;
            draw_setfb(32, 16);
            draw_filter = 0xaaaa;
            draw_tilesList();

            /* Core Design copyright + press space to start */
            draw_tllst = (U8 *)screen_imaincdc;
            draw_setfb(64, 80);
            draw_filter = 0x5555;
            draw_tilesList();
#endif
#ifdef GFXST
            draw_pic(pic_splash);
#endif
            seq = 2;
            break;
        }
        case 2:  /* wait for key pressed or timeout */
        {
            if (!sysarg_args_coins && control_test(Control_FIRE))
                seq = 3;
            else if (sys_gettime() - tm > SCREEN_TIMEOUT) {
                seen++;
                seq = 4;
            }
            break;
        }
        case 3:  /* wait for key released */
        {
            if (!(control_test(Control_FIRE))) {
                if (seen++ == 0)
                    seq = 4;
                else
                    seq = 7;
            }
            break;
        }
        case 4:  /* dispay hall of fame */
        {
            U8 s[32];
            size_t i;

            sysvid_clear();
            tm = sys_gettime();
            /* hall of fame title */
#ifdef GFXPC
            draw_tllst = (U8 *)screen_imainhoft;
            draw_setfb(32, 0);
            draw_filter = 0xaaaa;
            draw_tilesList();
#endif
#ifdef GFXST
            draw_pic(pic_haf);
#endif
            /* hall of fame content */
            draw_setfb(56, 48);
#ifdef GFXPC
            draw_filter = 0x5555;
#endif
            for (i = 0; i < screen_nbr_hiscores; i++) {
                sys_snprintf((char *)s, sizeof(s), "%06d@@@====@@@%s",
                    screen_highScores[i].score, screen_highScores[i].name);
                s[26] = (U8)'\377';
                s[27] = (U8)'\377';
                s[28] = (U8)'\376';
                draw_tllst = s;
                draw_tilesList();
            }
            seq = 5;
            break;
        }
        case 5:  /* wait for key pressed or timeout */
        {
            if (!sysarg_args_coins && control_test(Control_FIRE))
                seq = 6;
            else if (sys_gettime() - tm > SCREEN_TIMEOUT) {
                seen++;
                seq = 1;
            }
            break;
        }
        case 6:  /* wait for key released */
        {
            if (!(control_test(Control_FIRE))) {
                if (seen++ == 0)
                    seq = 1;
                else
                    seq = 7;
            }
            break;
        }
    }

    /*
     * coin mode: a coin inserted now starts a game right away, fire
     * starts one with a coin inserted before
     */
    if (sysarg_args_coins && seq != 7 && seq != 9) {
        if (seq == 8) {  /* wait for key released */
            if (!(control_test(Control_FIRE)))
                seq = 7;
        }
        else if (control_coins > coins_seen)
            seq = 7;
        else if (control_test(Control_FIRE) && control_coins > 0)
            seq = 8;
        coins_draw(newpic);
    }

    if (control_test(Control_EXIT))  /* check for exit request */
        return SCREEN_EXIT;

    if (seq == 7 || seq == 9) {  /* we're done */
        if (seq == 7 && sysarg_args_coins)
            control_coins--;  /* one coin per game */
        sysvid_clear();
        game_period = period;
        seen = 0;
        first = false;
        if (seq == 9) {  /* konami code: level select */
            seq = 0;
            return SCREEN_SELECT;
        }
        seq = 0;
        return SCREEN_DONE;
    }
    else
        return SCREEN_RUNNING;
}

/* eof */


