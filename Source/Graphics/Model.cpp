#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "ResourceManager.h"

// コンストラクタ
Model::Model(const char* filename)
{
    // リソース読み込み
    resource = ResourceManager::Instance().LoadModelResource(filename);

    // ノード
    const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

    nodes.resize(resNodes.size());
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        auto&& src = resNodes.at(nodeIndex);
        auto&& dst = nodes.at(nodeIndex);

        dst.name = src.name.c_str();
        dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
        dst.scale = src.scale;
        dst.rotate = src.rotate;
        dst.translate = src.translate;

        if (dst.parent != nullptr)
        {
            dst.parent->children.emplace_back(&dst);
        }
    }

    // 行列計算
    const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    UpdateTransform(transform);
}

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
    DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

    for (Node& node : nodes)
    {
        // ローカル行列算出
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
        DirectX::XMMATRIX LocalTransform = S * R * T;

        // ワールド行列算出
        DirectX::XMMATRIX ParentTransform;
        if (node.parent != nullptr)
        {
            ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
        }
        else
        {
            ParentTransform = Transform;
        }
        DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

        // 計算結果を格納
        DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
        DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
    }
}

// アニメーション更新処理
void Model::UpdateAnimation(float elapsedTime)
{
    // 再生中でないなら処理しない
    if (!IsPlayAnimation()) return;

    // ブレンド率の計算
    float blendRate = 1.0f;

    // ブレンドの準備を行う
    if (animationBlendTime < animationBlendSeconds) {
        animationBlendTime += elapsedTime;
        blendRate = animationBlendTime / animationBlendSeconds;
    }

    // 指定のアニメーションデータを取得
    const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
    const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

    // アニメーションデータからキーフレームデータリストを取得
    const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
    int keyCount = static_cast<int>(keyframes.size());

    // キーフレームが2つ以上ないと補間できない
    if (keyCount < 2) return;

    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        // 現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
        if (currentAnimationSeconds >= keyframe0.seconds &&
            currentAnimationSeconds < keyframe1.seconds)
        {
            // 再生時間とキーフレームの時間から補完率を算出する
            float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            // ノードごとに補完処理を行う
            int nodeCount = static_cast<int>(nodes.size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);
                Node& node = nodes[nodeIndex];

                // スケール補完
                DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
                DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
                DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);

                // 回転補完 (球面線形補間)
                DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
                DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
                DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);

                // 平行移動補完
                DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
                DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);
                DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

                // ブレンド率に基づいて最終的な値を決定
                if (blendRate < 1.0f)
                {
                    // 現在の姿勢とのブレンド
                    DirectX::XMVECTOR S_curr = DirectX::XMLoadFloat3(&node.scale);
                    DirectX::XMVECTOR R_curr = DirectX::XMLoadFloat4(&node.rotate);
                    DirectX::XMVECTOR T_curr = DirectX::XMLoadFloat3(&node.translate);

                    S = DirectX::XMVectorLerp(S_curr, S, blendRate);
                    R = DirectX::XMQuaternionSlerp(R_curr, R, blendRate);
                    T = DirectX::XMVectorLerp(T_curr, T, blendRate);
                }

                // 計算結果をノードに反映
                DirectX::XMStoreFloat3(&node.scale, S);
                DirectX::XMStoreFloat4(&node.rotate, R);
                DirectX::XMStoreFloat3(&node.translate, T);
            }
            break;
        }
    }

    // 時間経過
    currentAnimationSeconds += elapsedTime;

    // 再生時間が終端時間を超えたら
    if (currentAnimationSeconds >= animation.secondsLength)
    {
        if (animationLoopFlag)
        {
            // 再生時間を巻き戻す
            currentAnimationSeconds -= animation.secondsLength;
        }
        else
        {
            // 再生終了フラグを設定
            animationEndFlag = true;
            currentAnimationIndex = -1;
        }
    }
}

// アニメーション再生
void Model::PlayAnimation(int index, bool loop, float blendSeconds)
{
    currentAnimationIndex = animationNum = index;
    currentAnimationSeconds = 0.0f;
    animationLoopFlag = loop;
    animationEndFlag = false;

    animationBlendTime = 0.0f;
    animationBlendSeconds = blendSeconds;
}

// アニメーション再生中か
bool Model::IsPlayAnimation() const
{
    if (currentAnimationIndex < 0) return false;
    if (currentAnimationIndex >= resource->GetAnimations().size()) return false;
    return true;
}

// ノード検索
Model::Node* Model::FindNode(const char* name)
{
    // 全てのノードを総当たりで名前比較する
    for (Node& node : nodes)
    {
        if (strcmp(node.name, name) == 0)
        {
            return &node;
        }
    }
    // 見つからなかった
    return nullptr;
}
