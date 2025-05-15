#include "Graphics/Graphics.h"
#include "G2_Sundome.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G2_StageSundome.h"
#include "Game/Stage/StageMoveFloor.h"

//! ���[�r�[����
bool G2_Sundome::movieScene = false;

//! �X�R�A
int G2_Sundome::score[3] = { 0, 0, 0 };

// ������
void G2_Sundome::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// �����_�[�^�[�Q�b�g
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// �V���h�E�}�b�v������
	shadowMap.Initialize();

/********************************************************************************/

	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	std::unique_ptr<G2_StageSundome> stageMain = std::make_unique<G2_StageSundome>();
	stageManager.Register(std::move(stageMain));

	// �v���C���[������
	player = std::make_unique<Player2_Sundome>();
	player->SetPosition(DirectX::XMFLOAT3(215.0f, 70.0f, -2.0f));

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

	//! �|�X�g�G�t�F�N�g�̐ݒ�(��񂾂�)�t���O��������
	camera.SetPostEffectStatusOnceUnlock();

	//�J�����R���g���[���[������
	cameraController = std::make_unique <CameraController>();
	cameraController->SetTarget(DirectX::XMFLOAT3(-176.5, 61.5f, -2.8f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(45), 0, 0));
	cameraController->SetRange(42.5f);

	//! ���[�r�[�ɂ���
	movieScene = true;

	// �w�i
	backGround = std::make_unique<Sprite>();

	//! �t�F�[�h
	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);
}

// �I����
void G2_Sundome::Finalize()
{
	//! �X�e�[�W��j������
	StageManager::Instance().Clear();

	//! �G��S�Ĕj������
	EnemyManager::Instance().Clear();

	//! �G�t�F�N�g��S�Ĕj������
	EffectManager::Instance().Cleanup();
}

// �X�V����
void G2_Sundome::Update(float elapsedTime)
{
	//! �t�F�[�h�̍X�V����
	fade->Update(elapsedTime);

	//! ���[�r�[�X�V����
	UpdateMovie(elapsedTime);

	if(!movieScene || (movieScene && player->GetRound() >= 2))
	{
		// �J�����R���g���[���[�X�V����
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.y = 63.75f;

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
void G2_Sundome::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 15.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	//! �V���h�E�}�b�v
	shadowMapEyeOffset = { -18.0f, 53.0f, 9.0f };

	//! �t�H�O
	fogStart = 80.0f;
	fogEnd   = 130.0f;

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
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			backGroundColor.x, backGroundColor.y, backGroundColor.z, 1);

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

#ifndef _DEBUG

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

#endif // !_DEBUG

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

	{
		player->SpriteRender(dc, graphics);

		//! �t�F�[�h�̕`�揈��
		fade->Render(dc, graphics);
	}

#ifndef _DEBUG

	// 2D�f�o�b�OGUI�`��
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::ColorEdit3("BGColor", &backGroundColor.x);
			ImGui::InputFloat3("BGColorNum", &backGroundColor.x);

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

#endif // !_DEBUG
}

// ���[�r�[�X�V����
void G2_Sundome::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	switch (movieStep)
	{
	//! �J�����̈ʒu��ύX����
	case 0:

		//! ���[�r�[���Ԃ��v��
		movieTime += elapsedTime;

		if (movieTime > 2.0f)
		{
			// �J�����̈ʒu��ύX����
			cameraController->SetTargetChange(
				DirectX::XMFLOAT3(
					Player2_Sundome::Instance().GetPosition().x - 10.0f, 
					CameraController::target.y,
					CameraController::target.z),
					5.0f);

			//! ���[�r�[���Ԃ�0�ɂ���
			movieTime = 0.0f;

			movieStep++;
		}
		break;
	//! �v���C���[�ɋ߂Â���
	case 1:

		//! �J�����̃^�[�Q�b�g�̕ύX���I�������
		if (!CameraController::targetChange)
		{
			//! ���[�r�[���Ԃ��v��
			movieTime += elapsedTime;

			//! ���[�r�[���Ԃ�1���傫���Ȃ�
			if(movieTime > 1.0f)
			{
				DirectX::XMFLOAT3 pos = player->GetPosition();
				pos.y = 63.75f;

				//! �J�����̈ʒu��ύX����
				cameraController->SetTargetChange(pos, 2.0f);

				//! �J�����͈̔͂�ύX����
				cameraController->SetRangeChange(29.0f, 2.0f);

				//! ���[�r�[���Ԃ�0�ɂ���
				movieTime = 0.0f;

				movieStep++;
			}
		}
		break;
	//! ���[�r�[���I���
	case 2:

		if (!CameraController::targetChange)
		{
			//! ���[�r�[���Ԃ��v��
			movieTime += elapsedTime;

			if (movieTime > 1.0f)
			{
				movieScene = false;
				movieStep++;
			}
		}
		break;
	//! �߂郀�[�r�[
	case 3:

		movieTime = 0.0f;
		movieStep++;

		break;
	//! ���[�r�[�҂�����
	case 4:

		movieTime += elapsedTime;

		if (movieTime > 2.3f)
		{
			movieStep = 3;
			movieScene = false;
		}

		break;
	default:
		break;
	}
}