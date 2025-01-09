#pragma once

#include <memory>
#include <xaudio2.h>
#include "Audio/AudioResource.h"

// �I�[�f�B�I�\�[�X
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	~AudioSource();

	// �Đ�
	void Play(bool loop, float volume, float speed);

	// ��~
	void Stop();

	// BGM�̉��ʂ�X�s�[�h��ς���
	void ChangeBgmStatus(float volume = 1.0f, float speed = 1.0f);

	// �Đ�����
	bool IsPlaying() const;

private:
	IXAudio2SourceVoice*			sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
};
