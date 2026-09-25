/*
 * xrick/data/sounds.h
 *
 * Copyright (C) 2008-2014 Pierluigi Vicinanza. All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#ifndef _SOUNDS_H
#define _SOUNDS_H

#include "xrick/config.h"

#ifdef ENABLE_SOUND

#include "xrick/system/basic_types.h"

/*
 * A sound, as a WAVE file inside the data archive
 */
typedef struct {
  char *name;    /* file name in the data archive */
  U8 *buf;       /* samples, when loaded (see syssnd_load) */
  U32 len;       /* number of bytes in buf */
  bool dispose;  /* free buf after playing, false for cached sounds */
} sound_t;

enum
{
    /* expected format is 8-bit mono at 22050Hz */
    Wave_SAMPLE_RATE = 22050,
    Wave_AUDIO_FORMAT = 1, /* PCM = 1 (i.e. Linear quantization) */
    Wave_CHANNEL_COUNT = 1,
    Wave_BITS_PER_SAMPLE = 8,
};

/*
 * Header of a WAVE file, all numbers little endian
 */
typedef struct {
    /* "RIFF" chunk descriptor */
    U8 riffChunkId[4];
    U8 riffChunkSize[4];
    U8 riffType[4];
    /* "fmt" sub-chunk */
    U8 formatChunkId[4];
    U8 formatChunkSize[4];
    U8 audioFormat[2];
    U8 channelCount[2];
    U8 sampleRate[4];
    U8 byteRate[4];
    U8 blockAlign[2];
    U8 bitsPerSample[2];
    /* "data" sub-chunk */
    U8 dataChunkId[4];
    U8 dataChunkSize[4];
} wave_header_t;

/*
 * apparently there are 10 entity sounds in the original game (see the
 * e_them.c notes), however we only have 9 so far
 */
enum { SOUNDS_NBR_ENTITIES = 10 };

extern sound_t *soundBombshht;  /* dynamite fuse burning */
extern sound_t *soundBonus;     /* bonus picked up */
extern sound_t *soundBox;       /* box of bullets or dynamite picked up */
extern sound_t *soundBullet;    /* bullet fired */
extern sound_t *soundCrawl;     /* Rick crawling */
extern sound_t *soundDie;       /* Rick dies */
extern sound_t *soundEntity[SOUNDS_NBR_ENTITIES];  /* enemies, traps */
extern sound_t *soundExplode;   /* dynamite explodes */
extern sound_t *soundGameover;  /* game over tune */
extern sound_t *soundJump;      /* Rick jumps */
extern sound_t *soundPad;       /* Rick bounces on a super pad */
extern sound_t *soundSbonus1;   /* special bonus countdown starts */
extern sound_t *soundSbonus2;   /* special bonus won */
extern sound_t *soundStick;     /* Rick pokes with his stick */
extern sound_t *soundTune0;     /* tunes: the map intros play the one */
extern sound_t *soundTune1;     /* given by map_maps[].tune, the main */
extern sound_t *soundTune2;     /* intro plays tune 5 */
extern sound_t *soundTune3;
extern sound_t *soundTune4;
extern sound_t *soundTune5;
extern sound_t *soundWalk;      /* Rick walking */

#endif /* ENABLE_SOUND */

#endif /* ndef _SOUNDS_H */

/* eof */