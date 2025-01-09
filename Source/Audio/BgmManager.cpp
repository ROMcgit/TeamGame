#include "BgmManager.h"
#include <stdexcept>

// Bgmの読み込み
void BgmManager::LoadBgm(const std::string& name, const char* filename)
{
    // すでにキャッシュに存在する場合は再読み込みしない
    if (bgmList.find(name) == bgmList.end())
    {
        auto bgmSource = Audio::Instance().LoadAudioSource(filename);
        bgmList[name] = std::move(bgmSource);
    }
}

// Bgmの再生
void BgmManager::PlayBgm(const std::string& name, float volume, float speed, bool loop)
{
    // 現在再生中のBgmを停止
    if (currentBgm)
    {
        currentBgm->Stop();
    }

    // 新しいBgmの再生
    auto it = bgmList.find(name);
    if (it != bgmList.end())
    {
        currentBgm = it->second.get();
        currentBgmName = name;
        currentBgm->Play(loop, volume, speed);
    }
    else
        // 名前が間違っている場合に例外を投げる
        throw std::runtime_error("Bgmを呼び出すnameが間違っています！！");
}

// BGMの音量やスピードを変える
void BgmManager::ChangeBgmStatus(const std::string& name, float volume, float speed)
{
    // 新しいBgmの再生
    auto it = bgmList.find(name);
    if (it != bgmList.end())
    {
        currentBgm = it->second.get();
        currentBgmName = name;
        currentBgm->ChangeBgmStatus(volume, speed);
    }
    else
        // 名前が間違っている場合に例外を投げる
        throw std::runtime_error("Bgmを呼び出すnameが間違っています！！");
}

// Bgmの停止
void BgmManager::StopBgm()
{
    //! 再生中なら停止
    if (currentBgm)
    {
        currentBgm->Stop();
        currentBgm = nullptr;
        currentBgmName.clear();
    }
}

// Bgmの破棄
void BgmManager::UnloadBgm(const std::string& name)
{
    auto it = bgmList.find(name);
    if (it != bgmList.end())
    {
        // BGMの停止と破棄
        if (it->second.get() == currentBgm)
        {
            StopBgm();
        }
        bgmList.erase(it);
    }
}

// 音楽が再生中か
bool BgmManager::IsPlayingBgm() const
{
    if (currentBgm)
        return currentBgm->IsPlaying();

    return false;
}
