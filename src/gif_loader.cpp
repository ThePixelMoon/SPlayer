// gif_loader.cpp
#include "gif_loader.h"
#include "config.h"
#include <SDL2/SDL_image.h>
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