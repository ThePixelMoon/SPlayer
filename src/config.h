/*
	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

	For more information, please refer to <https://unlicense.org>
*/
#ifndef CONFIG_H
#define CONFIG_H

#pragma once

#include <SDL2/SDL.h>

static const int BASE_WIDTH = 400;
static const int BASE_HEIGHT = 400;
static const int MIN_SIZE = 200;
static const int MAX_SIZE = 800;
static const int CORNER_SIZE = 150;
static const int CORNER_PADDING = 50;

static const int BASELINE_MARGIN = 20;
static const int BASELINE_HEIGHT = 4;
static const int BASELINE_OFFSET = 40;

/* on win, windows can be placed on top of the explorer
   so, just make it 0. on the other hand, AKA, KDE,
   it cannot be placed on top of the 'explorer' or whatever
   its called so we set it to 40 for an optimal effect */
#ifdef _WIN32
static const int DEMOMAN_BOTTOM_MARGIN = 0;
#else
static const int DEMOMAN_BOTTOM_MARGIN = 40;
#endif

static const int AUDIO_FREQUENCY = 44100;
static const int AUDIO_CHUNKSIZE = 2048;

static const int DEFAULT_FRAME_DELAY = 100;

static const int MIN_DEMOMAN_SIZE = 32;
static const int MAX_DEMOMAN_SIZE = 256;
static const int DEMOMAN_SCALE_DIVISOR = 6;

/* buh. don't want to be using ! $ . ? and etc */
static const char *BASE_NAME = "SPlayer Window";

struct Color
{
	Uint8 r, g, b;
};

#endif // CONFIG_H