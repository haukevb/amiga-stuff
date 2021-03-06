#pragma once
#ifndef __COMMON_H__
#define __COMMON_H__

#include <exec/types.h>

struct PrgOptions {
    BOOL ntsc, how, speed, sky;
    WORD fetchmode;
};

struct FetchInfo
{
	WORD	ddfstart;
	WORD	ddfstop;
	WORD	modulooffset;
	WORD	bitmapoffset;
	WORD	scrollpixels;
};

struct RawMap
{
	WORD	mapwidth;
	WORD	mapheight;
	WORD	maplayers;
	WORD	blockwidth;
	WORD	blockheight;
	BYTE	bytesperblock;
	BYTE	transparentblock;
	UBYTE	data[1];
};

struct LevelMap {
    LONG width, height;
    UBYTE *data;
    struct RawMap *raw_map;
};

extern ULONG get_mode_id(BOOL option_how, BOOL option_ntsc);
extern BOOL get_arguments(struct PrgOptions *options, char *s);
extern BOOL read_level_map(const char * path, struct LevelMap *level_map, char *s);
extern struct BitMap *read_blocks(const char *path, UWORD *colors, char *s, int blocks_width, int blocks_height);

#endif /* __COMMON_H__ */
