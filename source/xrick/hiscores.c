/*
 * xrick/hiscores.c
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
 * Keeps the hall of fame in a plain text file, so that it survives
 * restarts and can be edited by hand. One entry per line:
 *
 *   SCORE NAME
 *
 * Lines that are empty or start with '#' are ignored. Names are upper
 * cased and may contain A-Z, 0-9, '.' and spaces; any other character
 * becomes a space. Entries are sorted by score. If the file lacks
 * entries, the remaining slots are cleared. If the file does not exist,
 * the hall of fame starts empty and the file is created, so deleting it
 * resets the high scores.
 */

#include "xrick/hiscores.h"

#ifdef ENABLE_HISCORE_FILE

#include "xrick/screens.h"
#include "xrick/system/system.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define HISCORES_MAX_SCORE 999999  /* six digits in the hall of fame */
#define NAME_BLANK '@'  /* blank tile */

/*
 * Convert one character typed in the file to a name tile
 */
static U8
name_fromText(int c)
{
  c = toupper(c);
  if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.')
    return (U8)c;
  return NAME_BLANK;
}

/*
 * Load the high scores from the text file
 */
void
hiscores_load(void)
{
  FILE *fp;
  char line[128];
  char *s, *end;
  unsigned long score;
  size_t n, i, j;
  hiscore_t tmp;

  n = 0;
  fp = fopen(HISCORES_FILE, "r");
  while (fp && n < screen_nbr_hiscores && fgets(line, sizeof(line), fp)) {
    s = line;
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0' || *s == '#') continue;

    score = strtoul(s, &end, 10);
    if (end == s) {
      s[strcspn(s, "\r\n")] = '\0';
      sys_printf("xrick/hiscores: ignoring invalid line \"%s\"\n", s);
      continue;
    }
    if (score > HISCORES_MAX_SCORE) score = HISCORES_MAX_SCORE;

    /* name is the rest of the line, after the separating blanks */
    s = end;
    while (*s == ' ' || *s == '\t') s++;
    for (i = 0; i < HISCORE_NAME_SIZE; i++) {
      if (*s == '\0' || *s == '\n' || *s == '\r')
        screen_highScores[n].name[i] = NAME_BLANK;
      else
        screen_highScores[n].name[i] = name_fromText((unsigned char)*s++);
    }
    screen_highScores[n].score = (U32)score;
    n++;
  }

  /* clear slots missing from the file */
  for (i = n; i < screen_nbr_hiscores; i++) {
    screen_highScores[i].score = 0;
    for (j = 0; j < HISCORE_NAME_SIZE; j++)
      screen_highScores[i].name[j] = NAME_BLANK;
  }

  /* inserting a new score expects them sorted, highest first */
  for (i = 1; i < screen_nbr_hiscores; i++) {
    tmp = screen_highScores[i];
    for (j = i; j > 0 && screen_highScores[j - 1].score < tmp.score; j--)
      screen_highScores[j] = screen_highScores[j - 1];
    screen_highScores[j] = tmp;
  }

  if (fp)
    fclose(fp);
  else
    hiscores_save();  /* no file yet: create an empty one */
}

/*
 * Save the high scores to the text file
 */
void
hiscores_save(void)
{
  FILE *fp;
  size_t i, j, len;
  char name[HISCORE_NAME_SIZE + 1];

  fp = fopen(HISCORES_FILE, "w");
  if (!fp) {
    sys_printf("xrick/hiscores: can not write \"%s\"\n", HISCORES_FILE);
    return;
  }

  fprintf(fp, "# xrick high scores, one entry per line: SCORE NAME\n");
  fprintf(fp, "# NAME: up to %d characters, A-Z, 0-9, '.' and spaces\n",
          HISCORE_NAME_SIZE);

  for (i = 0; i < screen_nbr_hiscores; i++) {
    len = 0;
    for (j = 0; j < HISCORE_NAME_SIZE; j++) {
      name[j] = (screen_highScores[i].name[j] == NAME_BLANK) ?
        ' ' : (char)screen_highScores[i].name[j];
      if (name[j] != ' ') len = j + 1;
    }
    name[len] = '\0';  /* drop trailing blanks */
    if (screen_highScores[i].score == 0 && len == 0)
      continue;  /* empty slot */
    fprintf(fp, "%06lu%s%s\n", (unsigned long)screen_highScores[i].score,
            len ? " " : "", name);
  }

  if (fclose(fp) != 0)
    sys_printf("xrick/hiscores: can not write \"%s\"\n", HISCORES_FILE);
}

#endif /* ENABLE_HISCORE_FILE */

/* eof */
