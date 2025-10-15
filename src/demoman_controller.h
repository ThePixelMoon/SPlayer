// demoman_controller.h
#ifndef DEMOMAN_CONTROLLER_H
#define DEMOMAN_CONTROLLER_H

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