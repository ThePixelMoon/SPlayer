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
#ifndef DEMOMAN_CONTROLLER_H
#define DEMOMAN_CONTROLLER_H

#pragma once

#include <SDL2/SDL.h>
#include "gif_loader.h"
#include "audio_manager.h"

class DemomanController
{
public:
	DemomanController( GifLoader *gifLoader, AudioManager *audioManager );

	void initialize( int screenWidth, int screenHeight );
	void handleMouseDown( int mx, int my, int screenWidth );
	void handleMouseUp( int screenWidth );
	void handleMouseMotion( int mx, int screenWidth );
	void update( int screenWidth, bool isDragging );
	void render( SDL_Renderer *renderer, int screenWidth, int screenHeight, double musicDuration );

	bool isDragging() const
	{
		return dragging;
	}

private:
	GifLoader	 *gifLoader;
	AudioManager *audioManager;

	int	 demX;
	int	 demY;
	int	 demWidth;
	int	 demHeight;
	bool dragging;
	int	 dragOffsetX;
};

#endif // DEMOMAN_CONTROLLER_H