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
#include "audio_manager.h"
#include "config.h"
#include <iostream>
#include <cmath>
#include <algorithm>

float AudioManager::targetVolume = 0.0f;

AudioManager::AudioManager() : music( nullptr ), musicDuration( -1.0 ), smoothVolume( 0.5f ), smoothBackgroundVolume( 0.0f ), isPlaying( false )
{
}

AudioManager::~AudioManager()
{
	isPlaying = false;

	if ( music )
		Mix_FreeMusic( music );

	Mix_CloseAudio();
}

bool AudioManager::initialize()
{
	if ( Mix_OpenAudio( AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, 2, AUDIO_CHUNKSIZE ) < 0 )
	{
		SDL_Log( "Mix_OpenAudio Error: %s\n", Mix_GetError() );
		return false;
	}

	Mix_SetPostMix( audioCallback, nullptr );
	return true;
}

bool AudioManager::loadMusic( const char *path )
{
	music = Mix_LoadMUS( path );
	if ( !music )
	{
		SDL_Log( "Mix_LoadMUS Error: %s\n", Mix_GetError() );
		return false;
	}

	musicDuration = Mix_MusicDuration( music );
	return true;
}

void AudioManager::play()
{
	if ( Mix_PlayMusic( music, -1 ) == -1 )
		SDL_Log( "Mix_PlayMusic failed: %s\n", Mix_GetError() );

	isPlaying = true;
}

void AudioManager::togglePause()
{
	if ( Mix_PausedMusic() )
	{
		isPlaying = true;
		Mix_ResumeMusic();
	}
	else
	{
		isPlaying = false;
		Mix_PauseMusic();
	}
}

bool AudioManager::seek( double position )
{
	if ( !canSeek() )
		return false;

	position = std::max( 0.0, std::min( musicDuration, position ) );

	if ( Mix_SetMusicPosition( position ) == -1 )
	{
		SDL_Log( "Mix_SetMusicPosition failed (format may not support seeking)\n" );
		return false;
	}
	return true;
}

double AudioManager::getDuration() const
{
	return musicDuration;
}

double AudioManager::getCurrentPosition() const
{
	if ( music )
		return Mix_GetMusicPosition( music );

	return -1.0;
}

bool AudioManager::canSeek() const
{
	return musicDuration > 0.0;
}

void AudioManager::updateSmoothing( float deltaTime )
{
	smoothVolume += ( targetVolume - smoothVolume ) * 0.3f;
	smoothBackgroundVolume += ( smoothVolume - smoothBackgroundVolume ) * 0.05f;
}

void AudioManager::audioCallback( void *userdata, Uint8 *stream, int len )
{
	Sint16 *samples = (Sint16 *)stream;
	int		sampleCount = len / 2;

	if ( sampleCount <= 0 )
		return;

	float sum = 0.0f;
	for ( int i = 0; i < sampleCount; i++ )
	{
		float sample = samples[i] / 32768.0f;
		sum += sample * sample;
	}

	float rms = std::sqrt( sum / sampleCount );
	targetVolume = rms * 1.5f;
	if ( targetVolume > 1.0f )
		targetVolume = 1.0f;
}