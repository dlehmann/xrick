/*
 * xrick/system/system.h
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
 * The system layer: everything the game needs from the platform (main
 * loop services, memory, video, files, events, keyboard, sound, command
 * line and joystick). Each section is implemented in a sysxxx_sdl.c
 * file for SDL, and in a sysxxx_rockbox.c file for Rockbox.
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

/*
 * Detect GCC
 */
#ifdef __GNUC__
/*
 * make POSIX functions available
 */
#  ifndef _POSIX_SOURCE
#    define _POSIX_SOURCE
#  endif
#endif

/*
 * Detect Microsoft Visual C
 */
#ifdef _MSC_VER
/*
 * FIXME disable "integral size mismatch in argument; conversion supplied" warning
 * as long as the code has not been cleared -- there are so many of them...
 */
#pragma warning( disable : 4761 )
#endif

/*
 * Detect Microsoft Windows
 */
#if !defined( __WIN32__ ) && ( defined( WIN32 ) || defined( _WIN32 ) )
#define __WIN32__
#endif

#include "xrick/config.h"
#include "xrick/rects.h"
#include "xrick/data/img.h"
#ifdef ENABLE_SOUND
#include "xrick/data/sounds.h"
#endif

#include <stddef.h> /* size_t */
#include <sys/types.h> /* off_t */

/*
 * main section
 */
extern bool sys_init(int, char **);  /* initialize all sections */
extern void sys_shutdown(void);
/*
 * let GCC and clang check the arguments against the format string, as
 * for printf
 */
#if defined(__GNUC__) || defined(__clang__)
#  define SYS_PRINTF_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#  define SYS_PRINTF_FORMAT(fmt, args)
#endif

extern void sys_error(const char *, ...)   /* report an error */
    SYS_PRINTF_FORMAT(1, 2);
extern void sys_printf(const char *, ...)  /* print to the console */
    SYS_PRINTF_FORMAT(1, 2);
extern void sys_snprintf(char *, size_t, const char *, ...)
    SYS_PRINTF_FORMAT(3, 4);
extern size_t sys_strlen(const char *);
extern U32 sys_gettime(void);  /* milliseconds since some fixed time */
extern void sys_yield(void);   /* let other tasks run a little */
extern bool sys_cacheData(void);  /* preload sounds before the game */
extern void sys_uncacheData(void);

/*
 * memory section: a stack allocator, blocks must be freed in the reverse
 * order of allocation
 */
extern bool sysmem_init(void);
extern void sysmem_shutdown(void);
extern void *sysmem_push(size_t);
extern void sysmem_pop(void *);

/*
 * video section
 */
#define SYSVID_ZOOM 2      /* default window zoom */
#define SYSVID_MAXZOOM 4
#define SYSVID_WIDTH 320   /* frame buffer size, pixels */
#define SYSVID_HEIGHT 200

extern U8 *sysvid_fb;  /* frame buffer, one byte (palette index) per pixel */

extern bool sysvid_init(void);
extern void sysvid_shutdown(void);
extern void sysvid_update(const rect_t *);  /* show these frame buffer parts */
extern void sysvid_clear(void);
extern void sysvid_zoom(S8);  /* zoom in (1) or out (-1) */
extern void sysvid_toggleFullscreen(void);
extern void sysvid_setGamePalette(void);
extern void sysvid_setPalette(img_color_t *, U16);

/*
 * file management section: data files, from a zip archive or a directory
 */
typedef void *file_t;

extern const char *sysfile_defaultPath;  /* data used without --data */

extern bool sysfile_setRootPath(const char *);  /* archive or directory */
extern void sysfile_dataPath(char *, size_t, const char *);  /* next to it */
extern void sysfile_clearRootPath(void);

extern file_t sysfile_open(const char *);
extern int sysfile_seek(file_t file, long offset, int origin);
extern int sysfile_tell(file_t);
extern off_t sysfile_size(file_t);
extern int sysfile_read(file_t, void *, size_t, size_t);
extern void sysfile_close(file_t);

/*
 * events section: turn keyboard, joystick and window events into
 * control_status
 */
extern void sysevt_poll(void);  /* process pending events */
extern void sysevt_wait(void);  /* wait for an event, then process events */

/*
 * keyboard section: key codes of the controls
 */
extern U16 syskbd_up;
extern U16 syskbd_down;
extern U16 syskbd_left;
extern U16 syskbd_right;
extern U16 syskbd_pause;
extern U16 syskbd_end;
extern U16 syskbd_xtra;  /* exit */
extern U16 syskbd_fire;
extern U16 syskbd_coin;  /* insert a coin (--coins) */
extern U16 syskbd_b;     /* B and A, for the konami code */
extern U16 syskbd_a;

/*
 * sound section
 */
#ifdef ENABLE_SOUND
extern const U8 syssnd_period; /* time between each sound update, in millisecond */

extern bool syssnd_init(void);
extern void syssnd_shutdown(void);
extern void syssnd_update(void);
extern void syssnd_vol(S8);  /* volume up (1) or down (-1) */
extern void syssnd_toggleMute(void);
extern void syssnd_play(sound_t *, S8);  /* play n times, -1 for ever */
extern void syssnd_pauseAll(bool);
extern void syssnd_stop(sound_t *);
extern void syssnd_stopAll(void);
#endif /* ENABLE_ SOUND */

/*
 * args section: command line options, see sysarg_help
 */
extern int sysarg_args_period;      /* ms per frame (--speed), 0: default */
extern int sysarg_args_map;         /* --map, 0 based */
extern int sysarg_args_submap;      /* --submap, 0 based, 0 for none */
extern int sysarg_args_fullscreen;  /* --fullscreen */
extern int sysarg_args_zoom;        /* --zoom, 0 for the default */
#ifdef ENABLE_SOUND
extern bool sysarg_args_nosound;    /* --nosound */
extern int sysarg_args_vol;         /* --vol, -1 when not given */
#endif /* ENABLE_ SOUND */
extern const char *sysarg_args_data;  /* --data, NULL for the default */
extern bool sysarg_args_coins;      /* --coins */
#ifdef ENABLE_LANG_FILE
extern const char *sysarg_args_lang;  /* --lang, NULL for the default */
#endif /* ENABLE_LANG_FILE */

extern bool sysarg_init(int, char **);  /* false: exit xrick */

/*
 * joystick section
 */
#ifdef ENABLE_JOYSTICK
extern bool sysjoy_init(void);
extern void sysjoy_shutdown(void);
#endif

#endif /* ndef _SYSTEM_H */

/* eof */


