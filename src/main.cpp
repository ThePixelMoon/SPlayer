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
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "audio_manager.h"
#include "gif_loader.h"
#include "visualizer.h"
#include "demoman_controller.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"

static bool showTechOverlay = false;

int main( int argc, char *argv[] )
{
	if ( argc < 2 )
	{
		SDL_Log( "Usage: %s <music file path>\n", argv[0] );
		return 1; /* error!! */
	}

	const char *musicPath = argv[1];

	std::srand( static_cast< unsigned int >( std::time( nullptr ) ) );
	Color globalColor;
	globalColor.r = std::rand() % 256;
	globalColor.g = std::rand() % 256;
	globalColor.b = std::rand() % 256;

	float bgColor[3] = { globalColor.r / 255.0f, globalColor.g / 255.0f, globalColor.b / 255.0f };

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

	SDL_Window	 *backgroundWindow = SDL_CreateWindow( "bg", 0, 0, dm.w, dm.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP );
	SDL_Renderer *backgroundRenderer = SDL_CreateRenderer( backgroundWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(backgroundWindow, backgroundRenderer);
	ImGui_ImplSDLRenderer2_Init(backgroundRenderer);

	SDL_Window *mainWindow = SDL_CreateWindow( BASE_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, BASE_WIDTH, BASE_HEIGHT, SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/ );
	SDL_SetWindowAlwaysOnTop( mainWindow, SDL_TRUE );
	SDL_Renderer *mainRenderer = SDL_CreateRenderer( mainWindow, -1, SDL_RENDERER_ACCELERATED );

	SDL_Rect usableBounds;
	SDL_GetDisplayUsableBounds( 0, &usableBounds );

	SDL_Window	 *corners[4];
	SDL_Renderer *cornerRenderers[4];

	/* 4corners windows */
	int x[4] = { CORNER_PADDING, usableBounds.w - CORNER_SIZE - CORNER_PADDING, CORNER_PADDING, usableBounds.w - CORNER_SIZE - CORNER_PADDING };
	int y[4] = { CORNER_PADDING, CORNER_PADDING, usableBounds.h - CORNER_SIZE - CORNER_PADDING, usableBounds.h - CORNER_SIZE - CORNER_PADDING };

	for ( int i = 0; i < 4; i++ )
	{
		corners[i] = SDL_CreateWindow( BASE_NAME, x[i], y[i], CORNER_SIZE, CORNER_SIZE, SDL_WINDOW_HIDDEN );
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

	SDL_RaiseWindow(backgroundWindow);
	SDL_SetWindowInputFocus(backgroundWindow);

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
    		ImGui_ImplSDL2_ProcessEvent(&event);

			switch ( event.type )
			{
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				if ( event.key.keysym.sym == SDLK_ESCAPE )
					running = false;
				else if ( event.key.keysym.sym == SDLK_SPACE )
					audioManager.togglePause();
				else if (event.key.keysym.sym == SDLK_F7)
					showTechOverlay = !showTechOverlay;
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
		visualizer.renderVolumeLines( backgroundRenderer, dm.w, dm.h );
		demoman.render( backgroundRenderer, dm.w, dm.h, audioManager.getDuration() );

		/* for imgui */
		globalColor.r = static_cast<Uint8>(bgColor[0] * 255);
		globalColor.g = static_cast<Uint8>(bgColor[1] * 255);
		globalColor.b = static_cast<Uint8>(bgColor[2] * 255);
		visualizer.SetColor(globalColor);

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (showTechOverlay)
		{
			ImGui::Begin("Technical Info", &showTechOverlay, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("Song: %s", musicPath);
			ImGui::Text("Length: %.2f seconds", audioManager.getDuration());
			ImGui::Text("Current volume: %.2f", audioManager.getSmoothVolume());
			ImGui::Text("Background volume: %.2f", audioManager.getBackgroundVolume());

    		ImGui::ColorEdit3("Background Color", bgColor);

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), backgroundRenderer);

		SDL_RenderPresent( backgroundRenderer );

		int mainWidth = BASE_WIDTH;
		int mainHeight = BASE_HEIGHT;
		visualizer.renderMainWindow( mainRenderer, mainWidth, mainHeight, audioManager.getSmoothVolume(), audioManager.getBackgroundVolume() );
		SDL_SetWindowSize( mainWindow, mainWidth, mainHeight );
		SDL_SetWindowPosition( mainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
		SDL_RenderPresent( mainRenderer );

		/* stick the corner windows */
		for ( int i = 0; i < 4; i++ )
		{
			int wx, wy;
			SDL_GetWindowPosition( corners[i], &wx, &wy );

			if ( wx != x[i] || wy != y[i] )
			{
				SDL_SetWindowPosition( corners[i], x[i], y[i] );
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
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer( mainRenderer );
	SDL_DestroyWindow( mainWindow );
	SDL_DestroyRenderer( backgroundRenderer );
	SDL_DestroyWindow( backgroundWindow );

	IMG_Quit();
	SDL_Quit();
	return 0;
}