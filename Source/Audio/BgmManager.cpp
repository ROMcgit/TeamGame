#include "BgmManager.h"
#include <stdexcept>

// Bgm�̓ǂݍ���
void BgmManager::LoadBgm(const std::string& name, const char* filename)
{
    // ���łɃL���b�V���ɑ��݂���ꍇ�͍ēǂݍ��݂��Ȃ�
    if (bgmList.find(name) == bgmList.end())
    {
        auto bgmSource = Audio::Instance().LoadAudioSource(filename);
        bgmList[name] = std::move(bgmSource);
    }
}

// Bgm�̍Đ�
void BgmManager::PlayBgm(const std::string& name, float volume, float speed, bool loop)
{
    // ���ݍĐ�����Bgm���~
    if (currentBgm)
    {
        currentBgm->Stop();
    }

    // �V����Bgm�̍Đ�
    auto it = bgmList.find(name);
    if (it != bgmList.end())
    {
        currentBgm = it->second.get();
        currentBgmName = name;
        currentBgm->Play(loop, volume, speed);
    }
    else
        // ���O���Ԉ���Ă���ꍇ�ɗ�O�𓊂���
        throw std::runtime_error("Bgm���Ăяo��name���Ԉ���Ă��܂��I�I");
}

// BGM�̉��ʂ�X�s�[�h��ς���
void BgmManager::ChangeBgmStatus(const std::string& name, float volume, float speed)
{
    // �V����Bgm�̍Đ�
    auto it = bgmList.find(name);
    if (it != bgmList.end())
    {
        currentBgm = it->second.get();
        currentBgmName = name;
        currentBgm->ChangeBgmStatus(volume, speed);
    }
    else
        // ���O���Ԉ���Ă���ꍇ�ɗ�O�𓊂���
        throw std::runtime_error("Bgm���Ăяo��name���Ԉ���Ă��܂��I�I");
}

// Bgm�̒�~
void BgmManager::StopBgm()
{
    //! �Đ����Ȃ��~
    if (currentBgm)
    {
        currentBgm->Stop();
        currentBgm = nullptr;
        currentBgmName.clear();
    }
}

// Bgm�̔j��
void BgmManager::UnloadBgm(const std::string& name)
{
    auto it = bgmList.find(name);
    if (it != bgmList.end())
    {
        // BGM�̒�~�Ɣj��
        if (it->second.get() == currentBgm)
        {
            StopBgm();
        }
        bgmList.erase(it);
    }
}

// ���y���Đ�����
bool BgmManager::IsPlayingBgm() const
{
    if (currentBgm)
        return currentBgm->IsPlaying();

    return false;
}
