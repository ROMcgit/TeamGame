#include "SoundManager.h"
#include <stdexcept>

// 効果音の読み込み
void SoundManager::LoadSound(const std::string& name, const char* filename)
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
void SoundManager::PlaySound(const std::string& name, float volume, float speed, bool loop)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        currentSound = it->second.get(); // 現在再生中の効果音を設定
        currentSound->Play(loop, volume, speed);
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
void SoundManager::StopSound(const std::string& name)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end())
    {
        it->second->Stop();
        // 停止時に再生中の音をリセット
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
        // currentSoundと比較を行う前に、イテレータが無効化されるのを避ける
        if (it->second.get() == currentSound)
        {
            currentSound = nullptr;
        }

        // 効果音を停止してから削除
        it->second->Stop();
        soundEffects.erase(it);

        // accessListから削除（要素が存在する場合のみ）
        auto listIt = std::find(accessList.begin(), accessList.end(), name);
        if (listIt != accessList.end())
        {
            accessList.erase(listIt);
        }
    }
}

// 効果音が再生中か
bool SoundManager::IsPlayingSound(const std::string& name) const
{
    return currentSound != nullptr && currentSound->IsPlaying();
}

// 効果音の音量やスピードを変える
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

// LRUキャッシュを管理
void SoundManager::ManageCacheSize()
{
    // キャッシュサイズが制限を超えたら、最も古い効果音を削除
    while (soundEffects.size() > MAX_CACHE_SIZE)
    {
        if (accessList.empty())
        {
            throw std::runtime_error("Access list is empty, but cache size exceeded!");
        }

        std::string lruSoundEffect = accessList.back(); // 最も古く使われた効果音

        // soundEffectsに存在しない場合はエラーをスローする（デバッグ用）
        if (soundEffects.find(lruSoundEffect) == soundEffects.end())
        {
            throw std::runtime_error("Access list and soundEffects are out of sync!");
        }
        accessList.pop_back();
        UnloadSound(lruSoundEffect);
    }
}
