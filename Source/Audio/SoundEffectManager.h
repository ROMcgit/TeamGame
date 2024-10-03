#pragma once

#include "Audio/Audio.h"
#include "Audio/AudioSource.h"
#include <unordered_map>
#include <string>

class SoundEffectManager
{
public:
    static const int MAX_CACHE_SIZE = 20; // キャッシュする効果音の最大数

    // シングルトンインスタンス取得
    static SoundEffectManager& Instance()
    {
        static SoundEffectManager instance;
        return instance;
    }

    // 効果音の読み込み
    void LoadSoundEffect(const std::string& name, const char* filename);
    
    // 効果音の再生
    void PlaySoundEffect(const std::string& name, float volume = 1.0f, float speed = 1.0f, bool loop = false);
    
    // 効果音の停止
    void StopSoundEffect(const std::string& name);
    
    // 効果音の破棄
    void UnloadSoundEffect(const std::string& name);

private:
    // LRUキャッシュを管理
    void ManageCacheSize();

private:

    // キャッシュされた効果音リスト
    std::unordered_map<std::string, std::unique_ptr<AudioSource>> soundEffects;

    // 最近使用された効果音の管理リスト
    std::list<std::string> accessList;
};
