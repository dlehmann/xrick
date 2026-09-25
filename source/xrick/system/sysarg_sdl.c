/*
 * xrick/system/sysarg_sdl.c
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
 * Command line options, SDL version. "xrick --help" lists them.
 *
 * 20021010 added test to prevent buffer overrun in -keys parsing.
 */

#include "xrick/system/system.h"
#include "xrick/config.h"
#include "xrick/game.h"

#include "xrick/maps.h"
#include "xrick/system/syssnd_sdl.h"

#include <stdlib.h>  /* atoi */
#include <string.h>  /* strcasecmp */

#include <SDL.h>

/* handle Microsoft Visual C */
#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

/* key names for --keys, and their SDL key codes */
typedef struct {
  char name[16];
  int code;
} sdlcodes_t;

static sdlcodes_t sdlcodes[SDLK_LAST] = {
#include "xrick/system/sdl_codes.e"
};

/* options, see system.h */
int sysarg_args_period = 0;
int sysarg_args_map = 0;
int sysarg_args_submap = 0;
int sysarg_args_fullscreen = 0;
int sysarg_args_zoom = 0;
bool sysarg_args_nosound = false;
int sysarg_args_vol = -1;  /* not given */
const char *sysarg_args_data = NULL;
bool sysarg_args_coins = false;
#ifdef ENABLE_LANG_FILE
const char *sysarg_args_lang = NULL;
#endif /* ENABLE_LANG_FILE */

/*
 * Version info
 */
static void sysarg_version(void)
{
    sys_printf(
        "xrick version '%s'\n\n"
        " Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net).\n"
        " Copyright (C) 2008-2014 Pierluigi Vicinanza.\n"
        " All rights reserved.\n\n"
        " The use and distribution terms for this software are contained in the file\n"
        " named README, which can be found in the root of this distribution. By\n"
        " using this software in any fashion, you are agreeing to be bound by the\n"
        " terms of this license.\n\n", XRICK_VERSION_STR);
}

/*
 * Help, printed in parts: sys_printf formats into a buffer of limited size
 */
static void sysarg_help(void)
{
    sys_printf(
        "Usage: xrick [option(s)]\n"
        "\n"
        "Start xrick from the directory that holds data.zip, or tell it where\n"
        "the data is with --data. The README explains everything in detail.\n"
        "\n"
        "General:\n"
        "  -h, --help         Print this information and exit.\n"
        "  --version          Print the version and exit.\n"
        "\n");
    sys_printf(
        "Display:\n"
        "  --fullscreen       Start in fullscreen mode instead of a window.\n"
        "                     F1 switches while playing.\n"
        "  --zoom <zoom>      Window size: the game's 320x200 pixels times\n"
        "                     <zoom>, from 1 to %d. The default is %d.\n"
        "                     F2 and F3 zoom out and in while playing.\n"
        "\n",
        SYSVID_MAXZOOM, SYSVID_ZOOM);
    sys_printf(
        "Game:\n"
        "  --speed <speed>    Time between two frames in milliseconds, from\n"
        "                     1 (fastest) to 100 (slowest). The default is %d.\n"
        "  --map <map>        Start at world <map>, from 1 to %d:\n"
        "                     1 South America, 2 Egypt, 3 castle, 4 missile\n"
        "                     base. The default is 1.\n"
        "  --submap <submap>  Start at submap <submap>, from 1 to %d. This\n"
        "                     also chooses the world, so it overrides --map.\n"
        "  --coins            Arcade mode: every game needs a coin, inserted\n"
        "                     with the C key (up to 99). A coin inserted on\n"
        "                     the start screen starts a game; with coins left,\n"
        "                     fire starts one. Coins are lost at exit.\n"
        "\n",
        GAME_PERIOD, 5/*MAP_NBR_MAPS*/-1, 47/*MAP_NBR_SUBMAPS*/);
    /* TODO: remove hardcoded map/submap max counts because they are now loaded from resource files */
    sys_printf(
        "Keys:\n"
        "  --keys <left>-<right>-<up>-<down>-<fire>\n"
        "                     Keys for the five game controls, as key names\n"
        "                     (see assets/docs/KeyCodes), not case sensitive.\n"
        "                     The default is z-x-o-k-SPACE. Examples:\n"
        "                       --keys a-d-w-s-SPACE\n"
        "                       --keys LEFT-RIGHT-UP-DOWN-LCTRL\n"
        "                       --keys KP4-KP6-KP8-KP2-KP0\n"
        "                     The arrow keys always work too. Fixed keys:\n"
        "                     P pause, E end game, ESC quit, C coin, F1-F3\n"
        "                     display, F4-F6 sound, F7-F9 cheats.\n"
        "\n");
    sys_printf(
        "Data and files:\n"
        "  --data <archive>   The game data: a zip file, or a directory with\n"
        "                     the unpacked data. The default is data.zip in\n"
        "                     the directory where xrick is run from.\n"
        "                     highscores.txt and lang/ are looked for in the\n"
        "                     same directory as the zip file (or in the data\n"
        "                     directory itself).\n"
#ifdef ENABLE_LANG_FILE
        "  --lang <code>      Language of the in-game texts, read from\n"
        "                     lang/<code>.txt next to the data, e.g.\n"
        "                     --lang de for German. The default is en.\n"
#endif /* ENABLE_LANG_FILE */
        "\n");
#ifdef ENABLE_SOUND
    sys_printf(
        "Sound:\n"
        "  --nosound          Start without sound. F4 mutes while playing.\n"
        "  --vol <vol>        Volume at start, from 0 (silent) to %d. The\n"
        "                     default is %d. F5 and F6 change it while\n"
        "                     playing.\n"
        "\n",
        SYSSND_MAXVOL, SYSSND_MAXVOL);
#endif /* ENABLE_SOUND */
}

/*
 * Fail
 */
static void sysarg_fail(char *msg)
{
    sys_printf(
        "xrick: %s\n"
        " Use 'xrick --help' for a complete list of options.\n", msg);
}

/*
 * Get SDL key code
 *
 * k: key name, see sdl_codes.e
 * return: key code, 0 if unknown
 */
static int sysarg_sdlcode(char *k)
{
  int i, result;

  i = 0;
  result = 0;

  while (sdlcodes[i].code) {
    if (!strcasecmp(sdlcodes[i].name, k)) {
      result = sdlcodes[i].code;
      break;
    }
    i++;
  }

  return result;
}

/*
 * Scan key codes sequence: <left>-<right>-<up>-<down>-<fire> key names,
 * and set the syskbd_xxx key codes
 *
 * return: false if the sequence is invalid
 */
static bool sysarg_scankeys(const char *keys)
{
  char k[16];
  int i, j;

  i = 0;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < (int)sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_left = sysarg_sdlcode(k);
  if (!syskbd_left) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < (int)sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_right = sysarg_sdlcode(k);
  if (!syskbd_right) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < (int)sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_up = sysarg_sdlcode(k);
  if (!syskbd_up) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < (int)sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_down = sysarg_sdlcode(k);
  if (!syskbd_down) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < (int)sizeof k) k[j++] = keys[i++];
  if (keys[i] != '\0') return false;
  k[j] = '\0';
  syskbd_fire = sysarg_sdlcode(k);
  if (!syskbd_fire) return false;

  return true;
}

/*
 * Read and process arguments
 *
 * return: false to exit xrick (error, help or version)
 */
bool
sysarg_init(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--fullscreen"))
        {
            sysarg_args_fullscreen = 1;
        }
        else if (!strcmp(argv[i], "--help") ||
                 !strcmp(argv[i], "-h"))
        {
            sysarg_help();
            return false;
        }
        else if (!strcmp(argv[i], "--speed"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing speed value");
                return false;
            }
            sysarg_args_period = atoi(argv[i]);
            if (sysarg_args_period < 1 || sysarg_args_period > 100)
            {
                sysarg_fail("invalid speed value");
                return false;
            }
        }
        else if (!strcmp(argv[i], "--keys"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing key codes");
                return false;
            }
            if (!sysarg_scankeys(argv[i]))
            {
                sysarg_fail("invalid key codes");
                return false;
            }
        }
        else if (!strcmp(argv[i], "--zoom"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing zoom value");
                return false;
            }
            sysarg_args_zoom = atoi(argv[i]);
            if (sysarg_args_zoom < 1 || sysarg_args_zoom > SYSVID_MAXZOOM)
            {
                sysarg_fail("invalid zoom value");
                return false;
            }
        }
        else if (!strcmp(argv[i], "--map"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing map number");
                return false;
            }
            sysarg_args_map = atoi(argv[i]) - 1;
            if (sysarg_args_map < 0 || sysarg_args_map >= 5/*MAP_NBR_MAPS*/-1) /* TODO: remove hardcoded map max count */
            {
                sysarg_fail("invalid map number");
                return false;
            }
        }
        else if (!strcmp(argv[i], "--submap"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing submap number");
                return false;
            }
            sysarg_args_submap = atoi(argv[i]) - 1;
            if (sysarg_args_submap < 0 || sysarg_args_submap >= 47/*MAP_NBR_SUBMAPS*/) /* TODO: remove hardcoded submap max count */
            {
                sysarg_fail("invalid submap number");
                return false;
            }
        }
#ifdef ENABLE_SOUND
        else if (!strcmp(argv[i], "--vol"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing volume");
                return false;
            }
            sysarg_args_vol = atoi(argv[i]);
            if (sysarg_args_vol < 0 || sysarg_args_vol > SYSSND_MAXVOL)
            {
                sysarg_fail("invalid volume");
                return false;
            }
        }
        else if (!strcmp(argv[i], "--nosound"))
        {
            sysarg_args_nosound = true;
        }
#endif /* ENABLE_SOUND */
        else if (!strcmp(argv[i], "--data"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing data");
                return false;
            }
            sysarg_args_data = argv[i];
        }
        else if (!strcmp(argv[i], "--coins"))
        {
            sysarg_args_coins = true;
        }
#ifdef ENABLE_LANG_FILE
        else if (!strcmp(argv[i], "--lang"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing language");
                return false;
            }
            sysarg_args_lang = argv[i];
        }
#endif /* ENABLE_LANG_FILE */
        else if (!strcmp(argv[i], "--version"))
        {
            sysarg_version();
            return false;
        }
        else
        {
            char message[128];
            sys_snprintf(message, sizeof(message), "unrecognized option '%s'", argv[i]);
            sysarg_fail(message);
            return false;
        }
    }

    /* TODO: remove checks below based on hardcoded values.
    *       Add code to check sysarg_args_map and sysarg_args_submap against map/submap max counts
    *       (after these have been loaded from resource files).
    */

    /*
     * this is dirty (sort of): find the map of the submap; the first
     * submaps of maps 2 to 4 are 9, 20 and 38, and starting there is
     * the same as starting at the map
     */
    if (sysarg_args_submap > 0 && sysarg_args_submap < 9)
    {
        sysarg_args_map = 0;
    }
    if (sysarg_args_submap >= 9 && sysarg_args_submap < 20)
    {
        sysarg_args_map = 1;
    }
    if (sysarg_args_submap >= 20 && sysarg_args_submap < 38)
    {
        sysarg_args_map = 2;
    }
    if (sysarg_args_submap >= 38)
    {
        sysarg_args_map = 3;
    }
    if (sysarg_args_submap == 9 ||
        sysarg_args_submap == 20 ||
        sysarg_args_submap == 38)
    {
        sysarg_args_submap = 0;
    }
    return true;
}

/* eof */





