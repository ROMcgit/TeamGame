#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Game/Scene/SceneManager.h"

// ������
void SceneLoading::Initialize()
{
	// �X�v���C�g������
	sprite = std::make_unique<Sprite>("Data/Sprite/LoadingIcon.png");

	// �X���b�h�J�n
	thread = std::make_unique <std::thread>(LoadingThread, this);
}

// �I����
void SceneLoading::Finalize()
{
	//if (sprite != nullptr)
	//{
	//	delete sprite;
	//	sprite = nullptr;
	//}

	//// �X���b�h�I����
	//if (thread != nullptr)
	//{
	//	if (thread->joinable())
	//	{
	//		thread->join(); // �X���b�h�̏I����҂�
	//	}
	//	delete thread;
	//	thread = nullptr;
	//}
	if (thread->joinable())
	{
		thread->join(); // �X���b�h�̏I����҂�
	}
}

// �X�V����
void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	//if (thread->joinable())
	//{
	//	thread->join(); // �X���b�h�̏I����҂�
	//}

	// ���̃V�[���̏���������������V�[����؂�ւ���
	if (nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(std::move(nextScene));
	}
}

// �`�揈��
void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A&�����_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; //RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2D�X�v���C�g�`��
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		float positionX = screenWidth - textureWidth;
		float positionY = screenHeight - textureHeight;
		// �^�C�g���X�v���C�g�`��
		sprite->Render(dc,
			positionX, positionY, textureWidth, textureHeight,
			0, 0, textureWidth, textureHeight,
			angle,
			1, 1, 1, 1);
	}
}

//���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM�֐��̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	// ���̃V�[���̏��������s��
	if (scene->nextScene != nullptr)
	{
		scene->nextScene->Initialize();
	}

	// �X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	// ���̃V�[���̏��������ݒ�
	if (scene->nextScene != nullptr)
	{
		scene->nextScene->SetReady();
	}
	//scene->SetReady();
}