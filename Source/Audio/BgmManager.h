#pragma once

#include "Audio/Audio.h"
#include "Audio/AudioSource.h"
#include <unordered_map>
#include <string>

class BgmManager
{
public:
    static BgmManager& Instance()
    {
        static BgmManager instance;
        return instance;
    }

    // Bgm�̓ǂݍ���
    void LoadBgm(const std::string& name, const char* filename);

    // Bgm�̍Đ�
    void PlayBgm(const std::string& name, float volume = 1.0f, float speed = 1.0f, bool loop = true);

    // BGM�̉��ʂ�X�s�[�h��ς���
    void ChangeBgmStatus(const std::string& name, float volume = 1.0f, float speed = 1.0f);

    // Bgm�̒�~
    void StopBgm();

    // Bgm�̔j��
    void UnloadBgm(const std::string& name);

    // ���ݍĐ�����Bgm���擾
    const std::string& GetCurrentBgm() const
    {
        return currentBgmName;
    }

    // ���y���Đ�����
    bool IsPlayingBgm() const;

private:
    std::unordered_map<std::string, std::unique_ptr<AudioSource>> bgmList;
    AudioSource* currentBgm = nullptr;  // ���ݍĐ�����Bgm
    std::string currentBgmName;         // ���ݍĐ�����Bgm�̖��O
};
