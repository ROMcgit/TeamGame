#include "Graphics/Graphics.h"
#include "G1_DarumasangaKoronda.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G1_StageDarumasangaKoronda.h"
#include "Game/Stage/StageMoveFloor.h"

//! ���[�r�[����
bool G1_DarumasangaKoronda::movieScene = false;

// ������
void G1_DarumasangaKoronda::Initialize()
{
	//movieScene = true;

	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// �����_�[�^�[�Q�b�g
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// �V���h�E�}�b�v������
	shadowMap.Initialize();

/********************************************************************************/

	float posZ = 54.0f;
	// �X�e�[�W������
	for(int i = 0; i < 3; i ++)
	{
		StageManager& stageManager = StageManager::Instance();
		std::unique_ptr<G1_StageDarumasangaKoronda> stageMain = std::make_unique<G1_StageDarumasangaKoronda>();
		
		float posZ = 54.0f + (i * 90.0f);

		stageMain->SetPosition(DirectX::XMFLOAT3(0, 0, posZ));
		stageManager.Register(std::move(stageMain));
	}

	// �v���C���[������
	player = std::make_unique<Player1_DarumasangaKoronda>();
	player->SetPosition(DirectX::XMFLOAT3(0, 3.0f, 0));

	// �J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	//�J�����R���g���[���[������
	cameraController = std::make_unique <CameraController>();
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(26.5f), 0, 0));
	cameraController->SetRange(20.0f);

	// �w�i
	backGround = std::make_unique<Sprite>();

	//! �t�F�[�h
	fade = std::make_unique<Fade>();
	//! �t�F�[�h��ݒ�
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);
}

// �I����
void G1_DarumasangaKoronda::Finalize()
{
	//! �X�e�[�W��j������
	StageManager::Instance().Clear();

	//! �G��S�Ĕj������
	EnemyManager::Instance().Clear();

	//! �G�t�F�N�g��S�Ĕj������
	EffectManager::Instance().Cleanup();
}

// �X�V����
void G1_DarumasangaKoronda::Update(float elapsedTime)
{
	//! �t�F�[�h���X�V����
	fade->Update(elapsedTime);

	//! ���[�r�[�X�V����
	UpdateMovie(elapsedTime);

	// �J�����R���g���[���[�X�V����
	if(!movieScene)
	{
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.x = 0;
		target.y += player->GetHeight() * 0.5f;
		cameraController->SetTarget(target);
	}
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);

	// �v���C���[�X�V����
	player->Update(elapsedTime);

	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void G1_DarumasangaKoronda::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 5.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 4.0f, 17.0f, 9.0f };

	//! �t�H�O
	fogStart = 500.0f;
	fogEnd   = 1200.0f;

	Graphics& graphics = Graphics::Instance();

	DrawingSettings(graphics);

	//! �V���h�E�}�b�v
	{
		// �|�[�Y��ʂ���Ȃ��Ȃ�
		//if (pause->GetPause_BurioHuntersOpacity() < 1.0f)
		{
			//! �V���h�E�}�b�v�J�n
			shadowMap.Begin(rc);
			{
				Shader* shadowMapShader = graphics.GetShadowMapShader();
				shadowMapShader->Begin(dc, rc);

				//�G�l�~�[�`��
				EnemyManager::Instance().Render(dc, shadowMapShader);
				// �v���C���[�`��
				player->Render(dc, shadowMapShader);

				shadowMapShader->End(dc);
			}
			//! �V���h�E�}�b�v�I��
			shadowMap.End();
		}
	}

	//! �����_�[�^�[�Q�b�g
	{
		//! �����_�[�^�[�Q�b�g�ɕ`��J�n
		renderTarget->Begin();
	}

	//! 2D�X�v���C�g
	{
		// �[�x�𖳌��ɂ���
		ID3D11DepthStencilState* depthDisabledState = graphics.GetDepthDisabledState();
		dc->OMSetDepthStencilState(depthDisabledState, 0);

		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(backGround->GetTextureWidth());
		float textureHeight = static_cast<float>(backGround->GetTextureHeight());

		//! �w�i�`��
		backGround->Render(dc,
			0, 0, screenWidth, screenHeight * 0.8f,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		ID3D11DepthStencilState* depthEnabledState = graphics.GetDepthEnabledState();
		dc->OMSetDepthStencilState(depthEnabledState, 0);
	}

	// 3D���f���`��
	{
		Shader* shader = graphics.GetDefaultLitShader();
		shader->Begin(dc, rc);
		// �X�e�[�W�`��
		StageManager::Instance().Render(dc, shader);

		// �v���C���[�`��
		player->Render(dc, shader);

		// �J�����̈ʒu��`��
		cameraController->RenderCameraTarget(dc, shader);

		//�G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);

	}

	// 3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	//! �V�F�[�_�[���o��
	{
		//! �����_�[�^�[�Q�b�g�֕`��I��
		renderTarget->End();
		//! �X�N���[�����|�X�g�G�t�F�N�g�V�F�[�_�[�ŕ`��
		Camera::Instance().CreatePostEffect();
		Camera::Instance().SetPostEffectStatusOnce();
		//! �X�N���[�����|�X�g�G�t�F�N�g�V�F�[�_�[�ŕ`��
		renderTarget->Render();
	}

	// 3D�f�o�b�O�`��
	{
		// �v���C���[�f�o�b�O�v���~�e�B�u�`��
		player->DrawDebugPrimitive();

		// �G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		EnemyManager::Instance().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	{
		//! �t�F�[�h�̕`�揈��
		fade->Render(dc, graphics);
	}

	// 2D�f�o�b�OGUI�`��
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			// �v���C���[�f�o�b�O�`��
			player->DrawDebugGUI();
			//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-----------------------------------------------------------------------------------------------------//
						// !�J�����̃f�o�b�O�`��
			if (ImGui::TreeNode("Cameras"))
			{
				//-------------------------------------------------------------------------------------------------------
				ImGui::Spacing(); // ��s�󂯂�
				ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
				ImGui::Spacing(); // ��s�󂯂�
				//-------------------------------------------------------------------------------------------------------
				Camera::Instance().DrawDebugGUI(); // ! �J����

				ImGui::Spacing(); // ��s�󂯂�
				ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
				ImGui::Spacing(); // ��s�󂯂�

				cameraController->DrawDebugGUI(); // !�J�����R���g���[���[
				ImGui::TreePop();
			}
			//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-----------------------------------------------------------------------------------------------------//
			EnemyManager::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}
}

// ���[�r�[�X�V����
void G1_DarumasangaKoronda::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	switch (movieStep)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}