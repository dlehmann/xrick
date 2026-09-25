/*
 * xrick/lang.c
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
 * NOTES
 *
 * Replaces the in-game texts with those of a language file, so that the
 * game can be played in other languages. "--lang de" loads lang/de.txt
 * next to the game data (see sysfile_dataPath); without --lang,
 * lang/en.txt is loaded, which also serves as a template for new
 * languages. If that file is missing, the texts from the data archive are
 * used. The file is plain UTF-8 text made of sections:
 *
 *   [intro1] ... [intro5]  map intro texts: a title line, then up to
 *                          11 lines of text
 *   [gameover]             game over banner, one line
 *   [paused]               pause banner, one line
 *   [entername]            prompt on the high score name screen, one line
 *   [insertcoin]           start screen prompt in coin mode, one line
 *   [credits]              label of the coin counter in coin mode, one line
 *   [world1] ... [world4]  world names on the level select screen
 *   [worldnumber]          "world 1 of 4" line, '#' stands for the numbers
 *   [pressfire]            level select prompt, one line
 *
 * Lines starting with '#' are comments. Missing sections keep the texts
 * from the data archive. The game font only has A-Z, 0-9, ',', '.', '?'
 * and blanks: lower case is upper cased, German umlauts become AE, OE, UE
 * and SS, anything else becomes a blank. Lines are centered by the game
 * and cut when too long for the screen.
 */

#include "xrick/lang.h"

#ifdef ENABLE_LANG_FILE

#include "xrick/system/system.h"

#include <stdio.h>
#include <string.h>

#define TILE_BLANK '@'
#define TILE_EOL '\377'  /* end of line */
#define TILE_EOT '\376'  /* end of text */

enum {
  IMAP_WIDTH = 30,       /* intro text width, in tiles */
  IMAP_LINES = 12,       /* title + text lines */
  BANNER_WIDTH = 25,     /* game over/pause, drawn at x=120 */
  GAMEOVER_MINWIDTH = 13,
  PAUSED_MINWIDTH = 10,
  GETNAME_WIDTH = 30,    /* drawn at x=40 */
  COINS_WIDTH = 16,      /* between the logos of the title screen */
  WORLD_WIDTH = 26,      /* level select title, between two arrows */
  LINE_SIZE = 256
};

enum { SEC_NONE, SEC_IMAP, SEC_GAMEOVER = SEC_IMAP + LANG_NBR_IMAPTEXT,
       SEC_PAUSED, SEC_GETNAME, SEC_INSERTCOIN, SEC_CREDITS,
       SEC_WORLD, SEC_WORLDNUMBER = SEC_WORLD + LANG_NBR_WORLDS,
       SEC_PRESSFIRE };

/*
 * global vars
 */
U8 *lang_imaptext[LANG_NBR_IMAPTEXT];
U8 *lang_gameovertxt = NULL;
U8 *lang_pausedtxt = NULL;
U8 *lang_getnametxt = NULL;
U8 *lang_insertcointxt = NULL;
U8 *lang_creditstxt = NULL;
U8 *lang_worldtxt[LANG_NBR_WORLDS];
U8 *lang_worldnumbertxt = NULL;
U8 *lang_pressfiretxt = NULL;

/*
 * local vars
 */
static U8 imap_buf[LANG_NBR_IMAPTEXT][IMAP_LINES * (IMAP_WIDTH + 1) + 1];
static U8 gameover_buf[3 * (BANNER_WIDTH + 1) + 1];
static U8 paused_buf[3 * (BANNER_WIDTH + 1) + 1];
static U8 getname_buf[GETNAME_WIDTH + 1];
static U8 insertcoin_buf[COINS_WIDTH + 1];
static U8 credits_buf[COINS_WIDTH + 1];
static U8 world_buf[LANG_NBR_WORLDS][WORLD_WIDTH + 1];
static U8 worldnumber_buf[IMAP_WIDTH + 1];
static U8 pressfire_buf[IMAP_WIDTH + 1];

/* raw lines of the section being read */
static char sec_lines[IMAP_LINES][LINE_SIZE];
static int sec_nbr_lines;

static bool keep_hash = false;  /* keep '#' number placeholders */


/*
 * Convert a UTF-8 line to game tiles
 *
 * return: number of tiles
 */
static size_t
to_tiles(const char *s, U8 *tiles, size_t max)
{
  static const struct { U8 c; const char *t; } umlauts[] = {
    { 0x84, "AE" }, { 0x96, "OE" }, { 0x9c, "UE" },  /* Ä Ö Ü */
    { 0xa4, "AE" }, { 0xb6, "OE" }, { 0xbc, "UE" },  /* ä ö ü */
    { 0x9f, "SS" }                                   /* ß */
  };
  const U8 *u = (const U8 *)s;
  const char *t;
  size_t n = 0, i;
  U8 c;

  while (*u && *u != '\n' && *u != '\r') {
    c = *u++;
    t = NULL;
    if (c >= 0x80) {
      /* multi-byte char: umlaut or blank */
      if (c == 0xc3 && *u >= 0x80 && *u < 0xc0) {
        for (i = 0; i < sizeof(umlauts) / sizeof(umlauts[0]); i++)
          if (umlauts[i].c == *u) t = umlauts[i].t;
      }
      while (*u >= 0x80 && *u < 0xc0) u++;  /* skip continuation bytes */
      if (!t) t = "@";
    }
    else if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
    else if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
               c == ',' || c == '.' || c == '?' || (c == '#' && keep_hash)))
      c = TILE_BLANK;

    if (!t) {
      if (n < max) tiles[n] = c;
      n++;
    }
    else for (; *t; t++) {
      if (n < max) tiles[n] = (*t == '@') ? TILE_BLANK : (U8)*t;
      n++;
    }
  }

  if (n > max) {
    sys_printf("xrick/lang: line too long, cut: \"%s\"\n", s);
    n = max;
  }
  /* drop trailing blanks */
  while (n > 0 && tiles[n - 1] == TILE_BLANK) n--;
  return n;
}

/*
 * Append a line to a tiles list, centered within width
 */
static U8 *
put_line(U8 *d, const char *s, size_t width, U8 end)
{
  U8 tiles[LINE_SIZE];
  size_t n, i;

  n = to_tiles(s, tiles, width);
  for (i = 0; i < (width - n) / 2; i++) *d++ = TILE_BLANK;
  memcpy(d, tiles, n);
  d += n;
  *d++ = end;
  return d;
}

/*
 * Convert a line to a tiles list, as is
 */
static U8 *
make_text(U8 *buf, const char *s, size_t width)
{
  buf[to_tiles(s, buf, width)] = TILE_EOT;
  return buf;
}

/*
 * Build a banner: blank line, text, blank line
 */
static U8 *
make_banner(U8 *buf, const char *s, size_t minwidth)
{
  U8 tiles[LINE_SIZE];
  U8 *d = buf;
  size_t n, w, i;

  n = to_tiles(s, tiles, BANNER_WIDTH - 2);
  w = (n + 2 > minwidth) ? n + 2 : minwidth;
  for (i = 0; i < w; i++) *d++ = TILE_BLANK;
  *d++ = TILE_EOL;
  d = put_line(d, s, w, TILE_EOL);
  for (i = 0; i < w; i++) *d++ = TILE_BLANK;
  *d++ = TILE_EOT;
  return buf;
}

/*
 * Turn the lines read for a section into game texts
 */
static void
end_section(int sec)
{
  U8 *d;
  int i;

  /* drop trailing empty lines */
  while (sec_nbr_lines > 0 && sec_lines[sec_nbr_lines - 1][0] == '\0')
    sec_nbr_lines--;
  if (sec == SEC_NONE || sec_nbr_lines == 0)
    return;

  if (sec >= SEC_IMAP && sec < SEC_IMAP + LANG_NBR_IMAPTEXT) {
    d = imap_buf[sec - SEC_IMAP];
    for (i = 0; i < sec_nbr_lines; i++)
      d = put_line(d, sec_lines[i], IMAP_WIDTH,
                   i + 1 < sec_nbr_lines ? TILE_EOL : TILE_EOT);
    lang_imaptext[sec - SEC_IMAP] = imap_buf[sec - SEC_IMAP];
  }
  else if (sec == SEC_GAMEOVER)
    lang_gameovertxt = make_banner(gameover_buf, sec_lines[0],
                                   GAMEOVER_MINWIDTH);
  else if (sec == SEC_PAUSED)
    lang_pausedtxt = make_banner(paused_buf, sec_lines[0], PAUSED_MINWIDTH);
  else if (sec == SEC_GETNAME) {
    put_line(getname_buf, sec_lines[0], GETNAME_WIDTH, TILE_EOT);
    lang_getnametxt = getname_buf;
  }
  else if (sec == SEC_INSERTCOIN)
    lang_insertcointxt = make_text(insertcoin_buf, sec_lines[0], COINS_WIDTH);
  else if (sec == SEC_CREDITS)
    /* room for a blank and the number */
    lang_creditstxt = make_text(credits_buf, sec_lines[0], COINS_WIDTH - 3);
  else if (sec >= SEC_WORLD && sec < SEC_WORLD + LANG_NBR_WORLDS)
    lang_worldtxt[sec - SEC_WORLD] =
      make_text(world_buf[sec - SEC_WORLD], sec_lines[0], WORLD_WIDTH);
  else if (sec == SEC_WORLDNUMBER) {
    keep_hash = true;
    lang_worldnumbertxt = make_text(worldnumber_buf, sec_lines[0], IMAP_WIDTH);
    keep_hash = false;
  }
  else if (sec == SEC_PRESSFIRE)
    lang_pressfiretxt = make_text(pressfire_buf, sec_lines[0], IMAP_WIDTH);
}

/*
 * Map a section header to a section number
 */
static int
section(const char *s)
{
  int n;

  if (sscanf(s, "[intro%d]", &n) == 1 && n >= 1 && n <= LANG_NBR_IMAPTEXT)
    return SEC_IMAP + n - 1;
  if (!strncmp(s, "[gameover]", 10)) return SEC_GAMEOVER;
  if (!strncmp(s, "[paused]", 8)) return SEC_PAUSED;
  if (!strncmp(s, "[entername]", 11)) return SEC_GETNAME;
  if (!strncmp(s, "[insertcoin]", 12)) return SEC_INSERTCOIN;
  if (!strncmp(s, "[credits]", 9)) return SEC_CREDITS;
  if (sscanf(s, "[world%d]", &n) == 1 && n >= 1 && n <= LANG_NBR_WORLDS)
    return SEC_WORLD + n - 1;
  if (!strncmp(s, "[worldnumber]", 13)) return SEC_WORLDNUMBER;
  if (!strncmp(s, "[pressfire]", 11)) return SEC_PRESSFIRE;
  return SEC_NONE;
}

/*
 * Load a language file
 *
 * code: language code, e.g. "de" for lang/de.txt
 * return: true if the file could be read
 */
bool
lang_load(const char *code)
{
  FILE *fp;
  char name[64];
  char path[1024];
  char line[LINE_SIZE];
  int sec = SEC_NONE;
  size_t len;

  sys_snprintf(name, sizeof(name), "%s/%s.txt", LANG_DIR, code);
  sysfile_dataPath(path, sizeof(path), name);
  fp = fopen(path, "r");
  if (!fp) {
    sys_printf("xrick/lang: can not open \"%s\"\n", path);
    return false;
  }

  sec_nbr_lines = 0;
  while (fgets(line, sizeof(line), fp)) {
    len = strcspn(line, "\r\n");
    line[len] = '\0';
    if (line[0] == '#') continue;

    if (line[0] == '[') {
      end_section(sec);
      sec = section(line);
      if (sec == SEC_NONE)
        sys_printf("xrick/lang: unknown section %s\n", line);
      sec_nbr_lines = 0;
      continue;
    }

    /* skip empty lines at the start of a section */
    if (sec == SEC_NONE || (sec_nbr_lines == 0 && len == 0)) continue;
    if (sec_nbr_lines == IMAP_LINES ||
        (sec >= SEC_GAMEOVER && sec_nbr_lines == 1)) {
      if (len > 0)
        sys_printf("xrick/lang: too many lines, ignoring \"%s\"\n", line);
      continue;
    }
    strcpy(sec_lines[sec_nbr_lines++], line);
  }
  end_section(sec);

  fclose(fp);
  return true;
}

#endif /* ENABLE_LANG_FILE */

/* eof */
