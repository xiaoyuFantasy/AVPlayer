#include "stdafx.h"
#include "SDLSound.h"


CSDLSound::CSDLSound()
{
}


CSDLSound::~CSDLSound()
{
}

bool CSDLSound::InitAudio()
{
	SDL_Init(SDL_INIT_AUDIO);
	return true;
}

bool CSDLSound::OpenAudio(int sample_rate, int channel, int channel_layout)
{
	m_sampleRate = sample_rate;
	m_channels = channel;
	m_channelLayout = channel_layout;

	SDL_AudioSpec wanted_spec, spec;

	const char* env = SDL_getenv("SDL_AUDIO_CHANNELS");
	if (env)
	{
		m_channels = atoi(env);
		m_channelLayout = av_get_default_channel_layout(m_channels);
	}

	if (!m_channelLayout || m_channels != av_get_channel_layout_nb_channels(m_channelLayout))
	{
		m_channelLayout = av_get_default_channel_layout(m_channelLayout);
		m_channelLayout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}

	m_channels = av_get_channel_layout_nb_channels(m_channelLayout);
	
	wanted_spec.channels = m_channels;
	wanted_spec.freq = m_sampleRate;
	if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!");
		return false;
	}

	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
	wanted_spec.callback = audio_callback;
	wanted_spec.userdata = this;

	m_audioDevID = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
	if (!m_audioDevID)
	{
		av_log(NULL, AV_LOG_ERROR, "error channels or sample_rate!!!");
		return false;
	}

	if (spec.format != AUDIO_S16SYS)
	{
		av_log(NULL, AV_LOG_ERROR, "SDL advised audio format %d is not supported!", spec.format);
		return false;
	}

	if (spec.channels != wanted_spec.channels)
	{
		m_channelLayout = av_get_default_channel_layout(spec.channels);
		if (!m_channelLayout)
		{
			av_log(NULL, AV_LOG_ERROR, "SDL advised channel count %d is not supported!\n", spec.channels);
			return false;
		}
	}

	return true;
}

bool CSDLSound::CloseAudio()
{
	return false;
}

void CSDLSound::SetCallback(funcCallback callback)
{
}

void CSDLSound::Start()
{
	SDL_PauseAudioDevice(m_audioDevID, 0);
}

void CSDLSound::Pause()
{
	SDL_PauseAudioDevice(m_audioDevID, 1);
}

void CSDLSound::SetVolume(int value)
{
}

int CSDLSound::GetVolume()
{
	return 0;
}

void CSDLSound::audio_callback(void * userData, Uint8 * stream, int len)
{
}
