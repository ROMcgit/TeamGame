#include "SoundEffectManager.h"
#include <stdexcept>

// 効果音の読み込み
void SoundEffectManager::LoadSoundEffect(const std::string& name, const char* filename)
{
    // 効果音が既にキャッシュされていないか確認
    auto it = soundEffects.find(name);
    if (it == soundEffects.end())
    {
        auto audioSource = Audio::Instance().LoadAudioSource(filename);
        soundEffects[name] = std::move(audioSource);
        accessList.push_front(name); // 最近使用した効果音をリストの先頭に追加

        // キャッシュサイズを管理する
        ManageCacheSize();
    }
    else
    {
        // 既にロード済みの効果音を再利用する
        accessList.remove(name);
        accessList.push_front(name);
    }
}

// 効果音の再生
void SoundEffectManager::PlaySoundEffect(const std::string& name, float volume, float speed, bool loop)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        it->second->Play(loop, volume, speed);
        // 再生時にアクセス順を更新
        accessList.remove(name);
        accessList.push_front(name);
    }
    else
    {
        // 効果音が見つからない場合は例外を投げる
        throw std::runtime_error("SoundEffectを呼び出すnameが間違っています！！");
    }
}

// 効果音の停止
void SoundEffectManager::StopSoundEffect(const std::string& name)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        it->second->Stop();
    }
}

void SoundEffectManager::UnloadSoundEffect(const std::string& name)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        // 停止してから削除する
        it->second->Stop();
        soundEffects.erase(it);
        accessList.remove(name);
    }
}

// LRUキャッシュを管理
void SoundEffectManager::ManageCacheSize()
{
    // キャッシュサイズが制限を超えたら、最も古い効果音を削除
    if (soundEffects.size() > MAX_CACHE_SIZE)
    {
        std::string lruSoundEffect = accessList.back();  // 最も古く使われた効果音
        accessList.pop_back();
        UnloadSoundEffect(lruSoundEffect);
    }
}
