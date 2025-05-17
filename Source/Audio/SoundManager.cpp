#include "SoundManager.h"
#include <stdexcept>

// ���ʉ��̓ǂݍ���
void SoundManager::LoadSound(const std::string& name, const char* filename)
{
    // ���ʉ������ɃL���b�V������Ă��Ȃ����m�F
    auto it = soundEffects.find(name);
    if (it == soundEffects.end())
    {
        auto audioSource = Audio::Instance().LoadAudioSource(filename);
        soundEffects[name] = std::move(audioSource);
        accessList.push_front(name); // �ŋߎg�p�������ʉ������X�g�̐擪�ɒǉ�

        // �L���b�V���T�C�Y���Ǘ�����
        ManageCacheSize();
    }
    else
    {
        // ���Ƀ��[�h�ς݂̌��ʉ����ė��p����
        accessList.remove(name);
        accessList.push_front(name);
    }
}

// ���ʉ��̍Đ�
void SoundManager::PlaySound(const std::string& name, float volume, float speed, bool loop)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        currentSound = it->second.get(); // ���ݍĐ����̌��ʉ���ݒ�
        currentSound->Play(loop, volume, speed);
        // �Đ����ɃA�N�Z�X�����X�V
        accessList.remove(name);
        accessList.push_front(name);
    }
    else
    {
        // ���ʉ���������Ȃ��ꍇ�͗�O�𓊂���
        throw std::runtime_error("SoundEffect���Ăяo��name���Ԉ���Ă��܂��I�I");
    }
}

// ���ʉ��̒�~
void SoundManager::StopSound(const std::string& name)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        it->second->Stop();
        // ��~���ɍĐ����̉������Z�b�g
        if (it->second.get() == currentSound)
        {
            currentSound = nullptr;
        }
    }
}

void SoundManager::UnloadSound(const std::string& name)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        // currentSound�Ɣ�r���s���O�ɁA�C�e���[�^�������������̂������
        if (it->second.get() == currentSound)
        {
            currentSound = nullptr;
        }

        // ���ʉ����~���Ă���폜
        it->second->Stop();
        soundEffects.erase(it);

        // accessList����폜�i�v�f�����݂���ꍇ�̂݁j
        auto listIt = std::find(accessList.begin(), accessList.end(), name);
        if (listIt != accessList.end())
        {
            accessList.erase(listIt);
        }
    }
}

// ���ʉ����Đ�����
bool SoundManager::IsPlayingSound(const std::string& name) const
{
    return currentSound != nullptr && currentSound->IsPlaying();
}

// ���ʉ��̉��ʂ�X�s�[�h��ς���
void SoundManager::ChangeSoundStatus(const std::string& name, float volume, float speed)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        currentSound = it->second.get();
        currentSound->ChangeBgmStatus(volume, speed);
    }
    else
        throw std::runtime_error("");
}

// LRU�L���b�V�����Ǘ�
void SoundManager::ManageCacheSize()
{
    // �L���b�V���T�C�Y�������𒴂�����A�ł��Â����ʉ����폜
    while (soundEffects.size() > MAX_CACHE_SIZE)
    {
        if (accessList.empty())
        {
            throw std::runtime_error("Access list is empty, but cache size exceeded!");
        }

        std::string lruSoundEffect = accessList.back(); // �ł��Â��g��ꂽ���ʉ�

        // soundEffects�ɑ��݂��Ȃ��ꍇ�̓G���[���X���[����i�f�o�b�O�p�j
        if (soundEffects.find(lruSoundEffect) == soundEffects.end())
        {
            throw std::runtime_error("Access list and soundEffects are out of sync!");
        }
        accessList.pop_back();
        UnloadSound(lruSoundEffect);
    }
}
