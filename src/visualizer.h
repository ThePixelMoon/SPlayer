// visualizer.h
#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SDL2/SDL.h>
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

private:
	Color					 globalColor;
	std::vector< AudioLine > leftLines;
	std::vector< AudioLine > rightLines;

	Uint8 clampColor( float v );
};

#endif // VISUALIZER_H