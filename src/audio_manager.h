// audio_manager.h
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL_mixer.h>

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	bool initialize();
	bool loadMusic( const char *path );
	void play();
	void togglePause();
	bool seek( double position );

	double getDuration() const;
	double getCurrentPosition() const;
	bool   canSeek() const;

	/* volume tracking */
	float getTargetVolume() const
	{
		return targetVolume;
	}
	float getSmoothVolume() const
	{
		return smoothVolume;
	}
	float getBackgroundVolume() const
	{
		return smoothBackgroundVolume;
	}

	bool getIsPlaying() const
	{
		return isPlaying;
	}

	void updateSmoothing( float deltaTime );

	static void audioCallback( void *userdata, Uint8 *stream, int len );

private:
	Mix_Music *music;
	double	   musicDuration;

	static float targetVolume;
	float		 smoothVolume;
	float		 smoothBackgroundVolume;

	bool isPlaying;
};

#endif // AUDIO_MANAGER_H