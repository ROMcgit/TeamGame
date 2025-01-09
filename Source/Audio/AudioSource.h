#pragma once

#include <memory>
#include <xaudio2.h>
#include "Audio/AudioResource.h"

// オーディオソース
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	~AudioSource();

	// 再生
	void Play(bool loop, float volume, float speed);

	// 停止
	void Stop();

	// BGMの音量やスピードを変える
	void ChangeBgmStatus(float volume = 1.0f, float speed = 1.0f);

	// 再生中か
	bool IsPlaying() const;

private:
	IXAudio2SourceVoice*			sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
};
