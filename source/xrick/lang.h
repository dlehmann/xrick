/*
 * xrick/lang.h
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

#ifndef _LANG_H
#define _LANG_H

#include "xrick/system/basic_types.h"
#include "xrick/config.h"

#ifdef ENABLE_LANG_FILE

#define LANG_DIR "lang"
#define LANG_DEFAULT "en"
#define LANG_NBR_IMAPTEXT 5

/* texts from the language file, NULL when not translated */
extern U8 *lang_imaptext[LANG_NBR_IMAPTEXT];
extern U8 *lang_gameovertxt;
extern U8 *lang_pausedtxt;
extern U8 *lang_getnametxt;
extern U8 *lang_insertcointxt;
extern U8 *lang_creditstxt;

extern bool lang_load(const char *);

/* translated text if there is one, else the default text */
#define LANG_TEXT(translated, text) ((translated) ? (translated) : (text))

#else

#define LANG_TEXT(translated, text) (text)

#endif /* ENABLE_LANG_FILE */

#endif /* ndef _LANG_H */

/* eof */
