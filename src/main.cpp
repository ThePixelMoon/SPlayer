// main.cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "audio_manager.h"
#include "gif_loader.h"
#include "visualizer.h"
#include "demoman_controller.h"

int main( int argc, char *argv[] )
{
	if (argc < 2) {
		SDL_Log("Usage: %s <music file path>\n", argv[0]);
		return 1; /* error!! */
	}

	const char *musicPath = argv[1];

	std::srand( static_cast< unsigned int >( std::time( nullptr ) ) );
	Color globalColor;
	globalColor.r = std::rand() % 256;
	globalColor.g = std::rand() % 256;
	globalColor.b = std::rand() % 256;

	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER ) < 0 )
	{
		SDL_Log( "SDL_Init failed: %s\n", SDL_GetError() );
		return 1;
	}

	IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP );

	AudioManager audioManager;
	if ( !audioManager.initialize() )
	{
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	if ( !audioManager.loadMusic( musicPath ) )
	{
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	/* get them display info */
	SDL_DisplayMode dm;
	if ( SDL_GetCurrentDisplayMode( 0, &dm ) < 0 )
	{
		SDL_Log( "SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError() );
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	SDL_Window	 *backgroundWindow = SDL_CreateWindow( "bg", 0, 0, dm.w, dm.h, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS /*| SDL_WINDOW_FULLSCREEN*/ );
	SDL_Renderer *backgroundRenderer = SDL_CreateRenderer( backgroundWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	SDL_Window *mainWindow = SDL_CreateWindow( BASE_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, BASE_WIDTH, BASE_HEIGHT, SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/ );
	SDL_SetWindowAlwaysOnTop( mainWindow, SDL_TRUE );
	SDL_Renderer *mainRenderer = SDL_CreateRenderer( mainWindow, -1, SDL_RENDERER_ACCELERATED );

	SDL_Rect usableBounds;
	SDL_GetDisplayUsableBounds( 0, &usableBounds );

	SDL_Window	 *corners[4];
	SDL_Renderer *cornerRenderers[4];

	/* 4corners windows */
	int x[4] = {
		CORNER_PADDING,
		usableBounds.w - CORNER_SIZE - CORNER_PADDING,
		CORNER_PADDING,
		usableBounds.w - CORNER_SIZE - CORNER_PADDING
	};

	int y[4] = {
		CORNER_PADDING,
		CORNER_PADDING,
		usableBounds.h - CORNER_SIZE - CORNER_PADDING,
		usableBounds.h - CORNER_SIZE - CORNER_PADDING
	};

	for (int i = 0; i < 4; i++)
		corners[i] = SDL_CreateWindow(BASE_NAME, x[i], y[i], CORNER_SIZE, CORNER_SIZE, SDL_WINDOW_SHOWN);

	for ( int i = 0; i < 4; ++i )
	{
		SDL_SetWindowAlwaysOnTop( corners[i], SDL_TRUE );
		cornerRenderers[i] = SDL_CreateRenderer( corners[i], -1, SDL_RENDERER_ACCELERATED );
	}

	GifLoader gifLoader;
	if ( !gifLoader.load( "conga.gif", backgroundRenderer ) )
	{
		for ( int i = 0; i < 4; i++ )
		{
			SDL_DestroyRenderer( cornerRenderers[i] );
			SDL_DestroyWindow( corners[i] );
		}
		SDL_DestroyRenderer( mainRenderer );
		SDL_DestroyWindow( mainWindow );
		SDL_DestroyRenderer( backgroundRenderer );
		SDL_DestroyWindow( backgroundWindow );
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	Visualizer		  visualizer( globalColor );
	DemomanController demoman( &gifLoader, &audioManager );
	demoman.initialize( dm.w, dm.h );

	audioManager.play();

	Uint32 bgWindowID = SDL_GetWindowID( backgroundWindow );
	Uint64 lastTick = SDL_GetTicks64();

	bool	  running = true;
	SDL_Event event;

	while ( running )
	{
		Uint64 now = SDL_GetTicks64();
		Uint64 deltaMs = ( now - lastTick );
		lastTick = now;

		/* if the music is paused, so is the demoman */
		if ( audioManager.getIsPlaying() )
			gifLoader.update( deltaMs );

		while ( SDL_PollEvent( &event ) )
		{
			switch ( event.type )
			{
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				if ( event.key.keysym.sym == SDLK_ESCAPE )
				{
					running = false;
				}
				else if ( event.key.keysym.sym == SDLK_SPACE )
				{
					audioManager.togglePause();
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if ( event.button.windowID == bgWindowID && event.button.button == SDL_BUTTON_LEFT )
				{
					int winW, winH;
					SDL_GetWindowSize( backgroundWindow, &winW, &winH );
					demoman.handleMouseDown( event.button.x, event.button.y, winW );
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if ( event.button.button == SDL_BUTTON_LEFT )
				{
					int winW, winH;
					SDL_GetWindowSize( backgroundWindow, &winW, &winH );
					demoman.handleMouseUp( winW );
				}
				break;

			case SDL_MOUSEMOTION:
				if ( demoman.isDragging() && event.motion.windowID == bgWindowID )
				{
					int winW, winH;
					SDL_GetWindowSize( backgroundWindow, &winW, &winH );
					demoman.handleMouseMotion( event.motion.x, winW );
				}
				break;

			default:
				break;
			}
		}

		audioManager.updateSmoothing( deltaMs / 1000.0f );
		visualizer.updateLines( dm.w, dm.h, audioManager.getSmoothVolume() );
		demoman.update( dm.w, demoman.isDragging() );

		visualizer.renderBackground( backgroundRenderer, dm.w, dm.h, audioManager.getBackgroundVolume() );

		/* THICC (with extra C) line */
		SDL_SetRenderDrawColor(backgroundRenderer, 255, 255, 255, 255);
		SDL_Rect bottomLine = { 0, dm.h - (dm.h / 11) - DEMOMAN_BOTTOM_MARGIN, dm.w, dm.h / 6 };
		SDL_RenderFillRect(backgroundRenderer, &bottomLine);

		demoman.render( backgroundRenderer, dm.w, dm.h, audioManager.getDuration() );

		SDL_RenderPresent( backgroundRenderer );

		int mainWidth = BASE_WIDTH;
		int mainHeight = BASE_HEIGHT;
		visualizer.renderMainWindow( mainRenderer, mainWidth, mainHeight, audioManager.getSmoothVolume(), audioManager.getBackgroundVolume() );
		SDL_SetWindowSize( mainWindow, mainWidth, mainHeight );
		SDL_SetWindowPosition( mainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
		SDL_RenderPresent( mainRenderer );

		/* stick the corner windows */
		for (int i = 0; i < 4; i++) {
			int wx, wy;
			SDL_GetWindowPosition(corners[i], &wx, &wy);

			if (wx != x[i] || wy != y[i]) {
				SDL_SetWindowPosition(corners[i], x[i], y[i]);
			}
		}

		visualizer.renderCorners( cornerRenderers, audioManager.getSmoothVolume() );
		for ( int i = 0; i < 4; ++i )
			SDL_RenderPresent( cornerRenderers[i] );
	}

	for ( int i = 0; i < 4; ++i )
	{
		SDL_DestroyRenderer( cornerRenderers[i] );
		SDL_DestroyWindow( corners[i] );
	}
	SDL_DestroyRenderer( mainRenderer );
	SDL_DestroyWindow( mainWindow );
	SDL_DestroyRenderer( backgroundRenderer );
	SDL_DestroyWindow( backgroundWindow );

	IMG_Quit();
	SDL_Quit();
	return 0;
}