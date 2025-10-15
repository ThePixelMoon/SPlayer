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
#ifndef VISUALIZER_H
#define VISUALIZER_H

#pragma once

#include <SDL.h>
#include <vector>
#include "config.h"

struct AudioLine
{
	float x, y, width, height, speed, alpha;
};

class Visualizer
{
public:
	Visualizer( const Color &color );

	void updateLines( int screenWidth, int screenHeight, float volume );
	void renderBackground( SDL_Renderer *renderer, int width, int height, float backgroundVolume );
	void renderMainWindow( SDL_Renderer *renderer, int &width, int &height, float smoothVolume, float backgroundVolume );
	void renderCorners( SDL_Renderer *renderers[4], float smoothVolume );
	void renderVolumeLines( SDL_Renderer *renderer, int screenWidth, int screenHeight );

	void SetColor(Color color)
	{
		globalColor = color;
	}

private:
	Color					 globalColor;
	std::vector< AudioLine > leftLines;
	std::vector< AudioLine > rightLines;

	std::vector< float > volumeLines;
	int					 lineSpacing = 10;

	Uint8 clampColor( float v );
};

#endif // VISUALIZER_H