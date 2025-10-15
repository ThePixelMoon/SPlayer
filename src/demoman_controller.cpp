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
#include "demoman_controller.h"
#include "config.h"
#include <algorithm>

DemomanController::DemomanController( GifLoader *gifLoader, AudioManager *audioManager ) : gifLoader( gifLoader ), audioManager( audioManager ), demX( 0 ), demY( 0 ), demWidth( 0 ), demHeight( 0 ), dragging( false ), dragOffsetX( 0 )
{
}

void DemomanController::initialize( int screenWidth, int screenHeight )
{
	int animW = gifLoader->getWidth();
	int animH = gifLoader->getHeight();

	demWidth = std::clamp( animW / DEMOMAN_SCALE_DIVISOR, MIN_DEMOMAN_SIZE, MAX_DEMOMAN_SIZE );
	demHeight = std::clamp( animH / DEMOMAN_SCALE_DIVISOR, MIN_DEMOMAN_SIZE, MAX_DEMOMAN_SIZE );
	demX = 0;
	demY = screenHeight - demHeight - DEMOMAN_BOTTOM_MARGIN;
}

void DemomanController::handleMouseDown( int mx, int my, int screenWidth )
{
	SDL_Rect  demRect{ demX, demY, demWidth, demHeight };
	SDL_Point p = { mx, my };

	if ( SDL_PointInRect( &p, &demRect ) )
	{
		dragging = true;
		dragOffsetX = mx - demX;
	}
	else
	{
		/* baseline => seek */
		if ( audioManager->canSeek() )
		{
			double duration = audioManager->getDuration();
			double newPos = (double)mx / (double)std::max( 1, screenWidth - demWidth ) * duration;
			audioManager->seek( newPos );
		}
	}
}

void DemomanController::handleMouseUp( int screenWidth )
{
	if ( dragging && audioManager->canSeek() )
	{
		double duration = audioManager->getDuration();
		double newPos = (double)demX / (double)std::max( 1, screenWidth - demWidth ) * duration;
		audioManager->seek( newPos );
	}
	dragging = false;
}

void DemomanController::handleMouseMotion( int mx, int screenWidth )
{
	if ( dragging )
	{
		demX = mx - dragOffsetX;
		demX = std::clamp( demX, 0, std::max( 0, screenWidth - demWidth ) );
	}
}

void DemomanController::update( int screenWidth, bool isDraggingNow )
{
	if ( !isDraggingNow )
	{
		double curPos = audioManager->getCurrentPosition();
		double duration = audioManager->getDuration();

		if ( curPos >= 0.0 && duration > 0.0 )
		{
			demX = static_cast< int >( ( curPos / duration ) * ( screenWidth - demWidth ) );
		}
	}
}

void DemomanController::render( SDL_Renderer *renderer, int screenWidth, int screenHeight, double musicDuration )
{
#if 0
	SDL_Rect baseline{ BASELINE_MARGIN, screenHeight - BASELINE_OFFSET, screenWidth - 2 * BASELINE_MARGIN, BASELINE_HEIGHT };
	SDL_SetRenderDrawColor( renderer, 50, 50, 50, 255 );
	SDL_RenderFillRect( renderer, &baseline );
#endif

	/* demoOoOOOoman */
	SDL_Texture *frame = gifLoader->getCurrentFrame();
	SDL_Rect	 dst{ demX, demY, demWidth, demHeight };

	if ( frame )
		SDL_RenderCopy( renderer, frame, nullptr, &dst );
	else
	{
		SDL_SetRenderDrawColor( renderer, 200, 60, 60, 255 );
		SDL_RenderFillRect( renderer, &dst );
	}

#if 0
	if ( musicDuration > 0.0 )
	{
		double pos = audioManager->getCurrentPosition();
		if ( pos >= 0.0 )
		{
			int		 filledW = static_cast< int >( ( pos / musicDuration ) * ( screenWidth - demWidth ) );
			SDL_Rect filled{ BASELINE_MARGIN, screenHeight - BASELINE_OFFSET, std::clamp( filledW, 0, screenWidth - 2 * BASELINE_MARGIN ), BASELINE_HEIGHT };
			SDL_SetRenderDrawColor( renderer, 180, 180, 40, 255 );
			SDL_RenderFillRect( renderer, &filled );
		}
	}
#endif
}