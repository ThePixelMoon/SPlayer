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
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#pragma once

#include <SDL_mixer.h>

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