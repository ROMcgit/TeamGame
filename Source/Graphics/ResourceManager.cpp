#include "Graphics/Graphics.h"
#include "ResourceManager.h"

// ���f�����\�[�X�ǂݍ���
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
    // ���f��������
    auto it = models.find(filename);
    if (it != models.end())
    {
        // ��Q�Ƃ����L�|�C���^�ɕϊ�
        std::shared_ptr<ModelResource> existingModel = it->second.lock();
        if (existingModel)
        {
            // �L���ȃ��f�����\�[�X��Ԃ�
            return existingModel;
        }
    }

    // �V�K���f�����\�[�X�쐬���ǂݍ���
    auto model = std::make_shared<ModelResource>();
    model->Load(Graphics::Instance().GetDevice(), filename);

    // �}�b�v�ɓo�^
    models[filename] = model;

    return model;
}