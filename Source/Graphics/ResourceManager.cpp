#include "Graphics/Graphics.h"
#include "ResourceManager.h"

// モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
    // モデルを検索
    auto it = models.find(filename);
    if (it != models.end())
    {
        // 弱参照を共有ポインタに変換
        std::shared_ptr<ModelResource> existingModel = it->second.lock();
        if (existingModel)
        {
            // 有効なモデルリソースを返す
            return existingModel;
        }
    }

    // 新規モデルリソース作成＆読み込み
    auto model = std::make_shared<ModelResource>();
    model->Load(Graphics::Instance().GetDevice(), filename);

    // マップに登録
    models[filename] = model;

    return model;
}