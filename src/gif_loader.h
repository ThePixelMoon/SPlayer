// gif_loader.h
#ifndef GIF_LOADER_H
#define GIF_LOADER_H

#include <SDL2/SDL.h>
#include <vector>

class GifLoader
{
public:
	GifLoader();
	~GifLoader();

	bool		 load( const char *path, SDL_Renderer *renderer );
	void		 update( Uint64 deltaMs );
	SDL_Texture *getCurrentFrame() const;

	int getWidth() const
	{
		return width;
	}
	int getHeight() const
	{
		return height;
	}
	int getFrameCount() const
	{
		return frameCount;
	}

private:
	std::vector< SDL_Texture * > frames;
	std::vector< int >			 delays;

	int	   width;
	int	   height;
	int	   frameCount;
	int	   currentFrame;
	Uint64 frameAccMs;
};

#endif // GIF_LOADER_H