#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "ResourceManager.h"

// �R���X�g���N�^
Model::Model(const char* filename)
{
    // ���\�[�X�ǂݍ���
    resource = ResourceManager::Instance().LoadModelResource(filename);

    // �m�[�h
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

    // �s��v�Z
    const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    UpdateTransform(transform);
}

// �ϊ��s��v�Z
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
    DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

    for (Node& node : nodes)
    {
        // ���[�J���s��Z�o
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
        DirectX::XMMATRIX LocalTransform = S * R * T;

        // ���[���h�s��Z�o
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

        // �v�Z���ʂ��i�[
        DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
        DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
    }
}

// �A�j���[�V�����X�V����
void Model::UpdateAnimation(float elapsedTime)
{
    // �Đ����łȂ��Ȃ珈�����Ȃ�
    if (!IsPlayAnimation()) return;

    // �u�����h���̌v�Z
    float blendRate = 1.0f;

    // �u�����h�̏������s��
    if (animationBlendTime < animationBlendSeconds) {
        animationBlendTime += elapsedTime;
        blendRate = animationBlendTime / animationBlendSeconds;
    }

    // �w��̃A�j���[�V�����f�[�^���擾
    const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
    const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

    // �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
    const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
    int keyCount = static_cast<int>(keyframes.size());

    // �L�[�t���[����2�ȏ�Ȃ��ƕ�Ԃł��Ȃ�
    if (keyCount < 2) return;

    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        // ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
        const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
        if (currentAnimationSeconds >= keyframe0.seconds &&
            currentAnimationSeconds < keyframe1.seconds)
        {
            // �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
            float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            // �m�[�h���Ƃɕ⊮�������s��
            int nodeCount = static_cast<int>(nodes.size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);
                Node& node = nodes[nodeIndex];

                // �X�P�[���⊮
                DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
                DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
                DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);

                // ��]�⊮ (���ʐ��`���)
                DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
                DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
                DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);

                // ���s�ړ��⊮
                DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
                DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);
                DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

                // �u�����h���Ɋ�Â��čŏI�I�Ȓl������
                if (blendRate < 1.0f)
                {
                    // ���݂̎p���Ƃ̃u�����h
                    DirectX::XMVECTOR S_curr = DirectX::XMLoadFloat3(&node.scale);
                    DirectX::XMVECTOR R_curr = DirectX::XMLoadFloat4(&node.rotate);
                    DirectX::XMVECTOR T_curr = DirectX::XMLoadFloat3(&node.translate);

                    S = DirectX::XMVectorLerp(S_curr, S, blendRate);
                    R = DirectX::XMQuaternionSlerp(R_curr, R, blendRate);
                    T = DirectX::XMVectorLerp(T_curr, T, blendRate);
                }

                // �v�Z���ʂ��m�[�h�ɔ��f
                DirectX::XMStoreFloat3(&node.scale, S);
                DirectX::XMStoreFloat4(&node.rotate, R);
                DirectX::XMStoreFloat3(&node.translate, T);
            }
            break;
        }
    }

    // ���Ԍo��
    currentAnimationSeconds += elapsedTime;

    // �Đ����Ԃ��I�[���Ԃ𒴂�����
    if (currentAnimationSeconds >= animation.secondsLength)
    {
        if (animationLoopFlag)
        {
            // �Đ����Ԃ������߂�
            currentAnimationSeconds -= animation.secondsLength;
        }
        else
        {
            // �Đ��I���t���O��ݒ�
            animationEndFlag = true;
            currentAnimationIndex = -1;
        }
    }
}

// �A�j���[�V�����Đ�
void Model::PlayAnimation(int index, bool loop, float blendSeconds)
{
    currentAnimationIndex = animationNum = index;
    currentAnimationSeconds = 0.0f;
    animationLoopFlag = loop;
    animationEndFlag = false;

    animationBlendTime = 0.0f;
    animationBlendSeconds = blendSeconds;
}

// �A�j���[�V�����Đ�����
bool Model::IsPlayAnimation() const
{
    if (currentAnimationIndex < 0) return false;
    if (currentAnimationIndex >= resource->GetAnimations().size()) return false;
    return true;
}

// �m�[�h����
Model::Node* Model::FindNode(const char* name)
{
    // �S�Ẵm�[�h�𑍓�����Ŗ��O��r����
    for (Node& node : nodes)
    {
        if (strcmp(node.name, name) == 0)
        {
            return &node;
        }
    }
    // ������Ȃ�����
    return nullptr;
}
