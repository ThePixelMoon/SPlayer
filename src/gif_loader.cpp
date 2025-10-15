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
#include "gif_loader.h"
#include "config.h"
#include <SDL_image.h>
#include <iostream>

GifLoader::GifLoader() : width( 0 ), height( 0 ), frameCount( 0 ), currentFrame( 0 ), frameAccMs( 0 )
{
}

GifLoader::~GifLoader()
{
	for ( auto tex : frames )
	{
		if ( tex )
		{
			SDL_DestroyTexture( tex );
		}
	}
}

bool GifLoader::load( const char *path, SDL_Renderer *renderer )
{
	IMG_Animation *anim = IMG_LoadAnimation( path );
	if ( !anim )
	{
		SDL_Log( "IMG_LoadAnimation failed (need SDL_image 2.6+/GIF not supported): %s\n", SDL_GetError() );
		return false;
	}

	width = anim->w;
	height = anim->h;
	frameCount = anim->count;

	frames.reserve( frameCount );
	delays.reserve( frameCount );

	for ( int i = 0; i < frameCount; ++i )
	{
		SDL_Surface *surf = anim->frames[i];
		if ( !surf )
		{
			frames.push_back( nullptr );
			delays.push_back( DEFAULT_FRAME_DELAY );
			continue;
		}

		SDL_Texture *tex = SDL_CreateTextureFromSurface( renderer, surf );
		if ( !tex )
		{
			SDL_Log( "SDL_CreateTextureFromSurface failed for frame %d: %s\n", i, SDL_GetError() );
			frames.push_back( nullptr );
		}
		else
		{
			frames.push_back( tex );
		}

		int delayMs = anim->delays[i];
		if ( delayMs <= 0 )
			delayMs = DEFAULT_FRAME_DELAY;
		delays.push_back( delayMs );
	}

	IMG_FreeAnimation( anim );
	return true;
}

void GifLoader::update( Uint64 deltaMs )
{
	if ( delays.empty() || frameCount == 0 )
		return;

	frameAccMs += deltaMs;
	int curDelay = delays[currentFrame];

	if ( (int)frameAccMs >= curDelay )
	{
		frameAccMs -= curDelay;
		currentFrame = ( currentFrame + 1 ) % frameCount;
	}
}

SDL_Texture *GifLoader::getCurrentFrame() const
{
	if ( frames.empty() || currentFrame >= (int)frames.size() )
		return nullptr;

	return frames[currentFrame];
}