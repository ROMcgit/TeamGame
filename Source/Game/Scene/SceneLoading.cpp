#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Game/Scene/SceneManager.h"

// ������
void SceneLoading::Initialize()
{
	int imageNum = rand() % 4;
	std::string backPath = "Data/Sprite/Loading/BackGround" + std::to_string(imageNum) + ".png";
	backGround = std::make_unique<Sprite>(backPath.c_str());

	// ���[�f�B���O
	for(int i = 0; i < 4; i++)
	{
		std::string filePath = "Data/Sprite/Loading/Loading" + std::to_string(i) + ".png";
		loading[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	// �X���b�h�J�n
	thread = std::make_unique <std::thread>(LoadingThread, this);

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f);
}

// �I����
void SceneLoading::Finalize()
{
	if (thread->joinable())
	{
		thread->join(); // �X���b�h�̏I����҂�
	}
}

// �X�V����
void SceneLoading::Update(float elapsedTime)
{
	fade->Update(elapsedTime);

	loadingTime += elapsedTime;

	if (loadingTime > 0.3f)
	{
		loadingTime = 0.0f;
		loadingNum++;
	}
	if (loadingNum > 3)
		loadingNum = 0;


	// ���̃V�[���̏���������������V�[����؂�ւ���
	if (nextScene->IsReady())
	{
		if (!setFade)
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				0.5f);

			setFade = true;
		}

		if(!fade->GetFade() && setFade)
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
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2D�X�v���C�g�`��
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		
		float textureWidth = static_cast<float>(backGround->GetTextureWidth());
		float textureHeight = static_cast<float>(backGround->GetTextureHeight());

		//! �w�i
		backGround->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			0.5f, 0.5f, 0.5f, 1);
		
		textureWidth = static_cast<float>(loading[loadingNum]->GetTextureWidth());
		textureHeight = static_cast<float>(loading[loadingNum]->GetTextureHeight());
		// ���[�f�B���O
		loading[loadingNum]->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		fade->Render(dc, graphics);
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