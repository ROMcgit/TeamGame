#pragma once

#include "Audio/Audio.h"
#include "Audio/AudioSource.h"
#include <unordered_map>
#include <string>

class SoundManager
{
public:
    static const int MAX_CACHE_SIZE = 50; // キャッシュする効果音の最大数

    // シングルトンインスタンス取得
    static SoundManager& Instance()
    {
        static SoundManager instance;
        return instance;
    }

    // 効果音の読み込み
    void LoadSound(const std::string& name, const char* filename);

    // 効果音の再生
    void PlaySound(const std::string& name, float volume = 1.0f, float speed = 1.0f, bool loop = false);

    // 効果音の停止
    void StopSound(const std::string& name);

    // 効果音の破棄
    void UnloadSound(const std::string& name);

    // 効果音が再生中か
    bool IsPlayingSound(const std::string& name) const;

private:
    // LRUキャッシュを管理
    void ManageCacheSize();

private:

    // キャッシュされた効果音リスト
    std::unordered_map<std::string, std::unique_ptr<AudioSource>> soundEffects;

    // 再生中の効果音
    AudioSource* currentSound = nullptr;

    // 最近使用された効果音の管理リスト
    std::list<std::string> accessList;
};