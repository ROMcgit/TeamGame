#include "Other/Misc.h"
#include "Audio/AudioSource.h"

// �R���X�g���N�^
AudioSource::AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource)
	: resource(resource)
{
	HRESULT hr;

	// �\�[�X�{�C�X�𐶐�
	hr = xaudio->CreateSourceVoice(&sourceVoice, &resource->GetWaveFormat());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// �f�X�g���N�^
AudioSource::~AudioSource()
{
	/*if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}*/
}

// �Đ�
void AudioSource::Play(bool loop, float volume, float speed)
{
	Stop();

	// �\�[�X�{�C�X�Ƀf�[�^�𑗐M
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->GetAudioBytes();
	buffer.pAudioData = resource->GetAudioData();
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	sourceVoice->SetVolume(volume);

	sourceVoice->SetFrequencyRatio(speed);
}

// ��~
void AudioSource::Stop()
{
	sourceVoice->FlushSourceBuffers();
	sourceVoice->Stop();
}

// BGM�̉��ʂ�X�s�[�h��ς���
void AudioSource::ChangeBgmStatus(float volume, float speed)
{
	sourceVoice->SetVolume(volume);
	sourceVoice->SetFrequencyRatio(speed);
}

// �Đ�����
bool AudioSource::IsPlaying() const
{
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state);

	return state.BuffersQueued > 0;  // �Đ����Ȃ�L���[�Ƀo�b�t�@���c���Ă���
}