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

    // Bgmの読み込み
    void LoadBgm(const std::string& name, const char* filename);
   
    // Bgmの再生
    void PlayBgm(const std::string& name, float volume = 1.0f, float speed = 1.0f, bool loop = true);

    // BGMの音量やスピードを変える
    void ChangeBgmStatus(const std::string& name, float volume = 1.0f, float speed = 1.0f);

    // Bgmの停止
    void StopBgm();
    
    // Bgmの破棄
    void UnloadBgm(const std::string& name);

    // 現在再生中のBgmを取得
    const std::string& GetCurrentBgm() const
    {
        return currentBgmName;
    }

    // 音楽が再生中か
    bool IsBgmPlaying() const;

private:
    std::unordered_map<std::string, std::unique_ptr<AudioSource>> bgmList;
    AudioSource* currentBgm = nullptr;  // 現在再生中のBgm
    std::string currentBgmName;         // 現在再生中のBgmの名前
};
