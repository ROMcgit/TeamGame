#pragma once

#include "Audio/Audio.h"
#include "Audio/AudioSource.h"
#include <unordered_map>
#include <string>

class SoundManager
{
public:
    static const int MAX_CACHE_SIZE = 50; // �L���b�V��������ʉ��̍ő吔

    // �V���O���g���C���X�^���X�擾
    static SoundManager& Instance()
    {
        static SoundManager instance;
        return instance;
    }

    // ���ʉ��̓ǂݍ���
    void LoadSound(const std::string& name, const char* filename);

    // ���ʉ��̍Đ�
    void PlaySound(const std::string& name, float volume = 1.0f, float speed = 1.0f, bool loop = false);

    // ���ʉ��̒�~
    void StopSound(const std::string& name);

    // ���ʉ��̔j��
    void UnloadSound(const std::string& name);

    // ���ʉ����Đ�����
    bool IsPlayingSound(const std::string& name) const;

    // ���ʉ��̉��ʂ�X�s�[�h��ς���
    void ChangeSoundStatus(const std::string& name, float volume = 1.0f, float speed = 1.0f);

private:
    // LRU�L���b�V�����Ǘ�
    void ManageCacheSize();

private:

    // �L���b�V�����ꂽ���ʉ����X�g
    std::unordered_map<std::string, std::unique_ptr<AudioSource>> soundEffects;

    // �Đ����̌��ʉ�
    AudioSource* currentSound = nullptr;

    // �ŋߎg�p���ꂽ���ʉ��̊Ǘ����X�g
    std::list<std::string> accessList;
};