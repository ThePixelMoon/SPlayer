// config.h
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
static const int DEMOMAN_BOTTOM_MARGIN = 40;

static const int AUDIO_FREQUENCY = 44100;
static const int AUDIO_CHUNKSIZE = 2048;

static const int DEFAULT_FRAME_DELAY = 100;

static const int MIN_DEMOMAN_SIZE = 32;
static const int MAX_DEMOMAN_SIZE = 256;
static const int DEMOMAN_SCALE_DIVISOR = 6;

/* buh. don't want to be using ! $ . ? and etc */
static const char* BASE_NAME = "SPlayer Window";

struct Color
{
	Uint8 r, g, b;
};

#endif // CONFIG_H