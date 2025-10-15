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
#include "visualizer.h"
#include <algorithm>
#include <cstdlib>

Visualizer::Visualizer( const Color &color ) : globalColor( color )
{
}

Uint8 Visualizer::clampColor( float v )
{
	if ( v < 0.f )
		return 0;
	if ( v > 255.f )
		return 255;
	return static_cast< Uint8 >( v );
}

void Visualizer::updateLines( int screenWidth, int screenHeight, float volume )
{
	/* create new lines or some shit idk */
	if ( std::rand() % 10 == 0 )
	{
		AudioLine newLine;
		newLine.y = std::rand() % screenHeight;
		newLine.width = 4;
		newLine.height = 20 + volume * 100;
		newLine.speed = 2 + volume * 5;
		newLine.alpha = 255;

		newLine.x = 0;
		leftLines.push_back( newLine );

		newLine.x = screenWidth - newLine.width;
		rightLines.push_back( newLine );
	}

	for ( auto &line : leftLines )
	{
		line.x += line.speed;
		line.alpha -= 4;
	}
	for ( auto &line : rightLines )
	{
		line.x -= line.speed;
		line.alpha -= 4;
	}

	/* the lines are dead */
	leftLines.erase( std::remove_if( leftLines.begin(), leftLines.end(), []( AudioLine &l ) { return l.alpha <= 0; } ), leftLines.end() );
	rightLines.erase( std::remove_if( rightLines.begin(), rightLines.end(), []( AudioLine &l ) { return l.alpha <= 0; } ), rightLines.end() );

	/* aaand voluem lines */
	int numLines = screenWidth / lineSpacing;
	volumeLines.resize( numLines );

	static float phase = 0.0f;
	phase += 0.05f;

	for ( int i = 0; i < numLines; ++i )
	{
		float wave = sin(i*0.3f + phase);
		volumeLines[i] = ((wave+1)/2) * (screenHeight/2) * volume;
	}
}

void Visualizer::renderBackground( SDL_Renderer *renderer, int width, int height, float backgroundVolume )
{
	Uint8 bgR = clampColor( globalColor.r * ( 0.1f + backgroundVolume * 0.4f ) );
	Uint8 bgG = clampColor( globalColor.g * ( 0.1f + backgroundVolume * 0.4f ) );
	Uint8 bgB = clampColor( globalColor.b * ( 0.1f + backgroundVolume * 0.4f ) );

	SDL_SetRenderDrawColor( renderer, bgR, bgG, bgB, 255 );
	SDL_RenderClear( renderer );

	for ( auto &line : leftLines )
	{
		SDL_SetRenderDrawColor( renderer, clampColor( globalColor.r * ( line.alpha / 255.0f ) ), clampColor( globalColor.g * ( line.alpha / 255.0f ) ), clampColor( globalColor.b * ( line.alpha / 255.0f ) ), 255 );
		SDL_Rect rect = { static_cast< int >( line.x ), static_cast< int >( line.y ), static_cast< int >( line.width ), static_cast< int >( line.height ) };
		SDL_RenderFillRect( renderer, &rect );
	}

	for ( auto &line : rightLines )
	{
		SDL_SetRenderDrawColor( renderer, clampColor( globalColor.r * ( line.alpha / 255.0f ) ), clampColor( globalColor.g * ( line.alpha / 255.0f ) ), clampColor( globalColor.b * ( line.alpha / 255.0f ) ), 255 );
		SDL_Rect rect = { static_cast< int >( line.x ), static_cast< int >( line.y ), static_cast< int >( line.width ), static_cast< int >( line.height ) };
		SDL_RenderFillRect( renderer, &rect );
	}
}

void Visualizer::renderVolumeLines( SDL_Renderer *renderer, int screenWidth, int screenHeight )
{
	SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

	for ( size_t i = 0; i < volumeLines.size(); ++i )
	{
		int x = static_cast< int >( i * lineSpacing );
		int lineHeight = static_cast< int >( volumeLines[i] );
		int yTop = screenHeight / 2 - lineHeight / 2;
		int yBottom = screenHeight / 2 + lineHeight / 2;

		SDL_RenderDrawLine( renderer, x, yTop, x, yBottom );
	}
}

void Visualizer::renderMainWindow( SDL_Renderer *renderer, int &width, int &height, float smoothVolume, float backgroundVolume )
{
	float scaleFactor = 0.6f + smoothVolume * smoothVolume * 1.5f;
	width = std::clamp( static_cast< int >( BASE_WIDTH * scaleFactor ), MIN_SIZE, MAX_SIZE );
	height = std::clamp( static_cast< int >( BASE_HEIGHT * scaleFactor ), MIN_SIZE, MAX_SIZE );

	Uint8 circleR = clampColor( globalColor.r * ( 0.25f + backgroundVolume * 0.5f ) );
	Uint8 circleG = clampColor( globalColor.g * ( 0.25f + backgroundVolume * 0.5f ) );
	Uint8 circleB = clampColor( globalColor.b * ( 0.25f + backgroundVolume * 0.5f ) );

	SDL_SetRenderDrawColor( renderer, circleR, circleG, circleB, 255 );
	SDL_RenderClear( renderer );

	Uint8 circleR1 = clampColor( globalColor.r * ( 0.75f + backgroundVolume * 0.5f ) );
	Uint8 circleG1 = clampColor( globalColor.g * ( 0.75f + backgroundVolume * 0.5f ) );
	Uint8 circleB1 = clampColor( globalColor.b * ( 0.75f + backgroundVolume * 0.5f ) );

	int centerX = width / 2;
	int centerY = height / 2;
	int radius = static_cast< int >( 50 + smoothVolume * smoothVolume * 200 );

	for ( int w = -radius; w <= radius; ++w )
	{
		for ( int h = -radius; h <= radius; ++h )
		{
			if ( w * w + h * h <= radius * radius )
			{
				SDL_SetRenderDrawColor( renderer, circleR1, circleG1, circleB1, 255 );
				SDL_RenderDrawPoint( renderer, centerX + w, centerY + h );
			}
		}
	}
}

void Visualizer::renderCorners( SDL_Renderer *renderers[4], float smoothVolume )
{
	for ( int i = 0; i < 4; ++i )
	{
		float brightnessFactor = 0.5f + smoothVolume * 1.15f;
		Uint8 cornerR = clampColor( globalColor.r * brightnessFactor );
		Uint8 cornerG = clampColor( globalColor.g * brightnessFactor );
		Uint8 cornerB = clampColor( globalColor.b * brightnessFactor );

		SDL_SetRenderDrawColor( renderers[i], cornerR, cornerG, cornerB, 255 );
		SDL_RenderClear( renderers[i] );
	}
}