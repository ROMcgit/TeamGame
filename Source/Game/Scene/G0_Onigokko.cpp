#include "Graphics/Graphics.h"
#include "G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G0_StageOnigokko.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Scene/SceneTitle.h"
#include "Game/Scene/G0_Onigokko_Clear.h"
#include "Game/Scene/G0_Onigokko_GameOver.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneManager.h"
#include <algorithm>

bool G0_Onigokko::movieScene = false;

// ������
void G0_Onigokko::Initialize()
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
	std::unique_ptr<G0_StageOnigokko> stageMain = std::make_unique<G0_StageOnigokko>();
	stageManager.Register(std::move(stageMain));

	// �v���C���[������
	player = std::make_unique<Player0_Onigokko>();
	player->SetPosition(DirectX::XMFLOAT3(0, 5.0f, 0));
	player->SetMovieTime(10.0f);
	player->SetMovieAnimation(0, true);

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
	cameraController->SetRange(30);
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(10), 0, 0));

	// �w�i
	backGround = std::make_unique<Sprite>();

	// �S
	std::unique_ptr<EnemyOni> oni = std::make_unique<EnemyOni>();
	oni->SetPosition(DirectX::XMFLOAT3(0, 5, 250));
	EnemyManager::Instance().Register(std::move(oni));

	fade = std::make_unique<Fade>();
	//! �t�F�[�h��ݒ�
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);

	// �^�C�}�[
	timer = std::make_unique<Timer>(true, 2);

	// ���[�r�[�V�[���ɂ���
	movieScene = true;
}

// �I����
void G0_Onigokko::Finalize()
{
	//! �X�e�[�W��j������
	StageManager::Instance().Clear();

	//! �G��S�Ĕj������
	EnemyManager::Instance().Clear();

	//! �G�t�F�N�g��S�Ĕj������
	EffectManager::Instance().Cleanup();
}

// �X�V����
void G0_Onigokko::Update(float elapsedTime)
{
	// �J�����R���g���[���[�X�V����
	if (!movieScene)
	{
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetRange(110);
		cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(40), 0, 0));
	}
	
	cameraController->SetTarget(target);
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	fade->Update(elapsedTime);

	if(!movieScene)
	{
		if(EnemyManager::Instance().GetEnemyCount() < 50)
		{
			// �S
			std::unique_ptr<EnemyOni> oni = std::make_unique<EnemyOni>();
			{
				oni->SetPosition(DirectX::XMFLOAT3(100, 5, 200));
				EnemyManager::Instance().Register(std::move(oni));
			}
		}

		// �^�C�}�[�̍X�V����
		timer->Update(elapsedTime);
	}

	// �X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);

	//! �v���C���[�̈ʒu����
	PlayerPositionControll();
	// �v���C���[�X�V����
	player->Update(elapsedTime);

	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);

	// �J�����̃��[�r�[�X�V����
	UpdateCameraMovie(elapsedTime);
}

// �`�揈��
void G0_Onigokko::Render()
{
	std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

	if (movieScene && cameraMovieScene != CameraMovieScene::OniMove)
		lightPosition = oni->GetPosition();
	else
		lightPosition = player->GetPosition();


	lightRange = 30.0f;

	//! �t�H�O
	fogStart = 20.0f;
	fogEnd = 100.0f;
	fogColor = { 0, 0, 0 };

	shadowMapEyeOffset = { 4.0f, 17.0f, 9.0f };

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

		// �J�����̈ʒu��`��
		CameraController::Instance().RenderCameraTarget(dc, shader);

		// �v���C���[�`��
		player->Render(dc, shader);

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
#endif // _DEBUG

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
		if (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() == 0)
			timer->Render(dc, graphics, DirectX::XMFLOAT2(30, 0), DirectX::XMFLOAT4(1, 1, 0, 1));
		else
			timer->Render(dc, graphics, DirectX::XMFLOAT2(30, 0));
	
		fade->Render(dc, graphics);
	}

#ifndef _DEBUG

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
#endif // _DEBUG
}

// �v���C���[�̈ʒu����
void G0_Onigokko::PlayerPositionControll()
{
	if (player->GetPosition().x < -445.0f || player->GetPosition().x > 445.0f)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -445.0f, 445.0f);
		player->SetPositionX(positoinX);
	}

	if (player->GetPosition().z < -445.0f || player->GetPosition().z > 445.0f)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -445.0f, 445.0f);
		player->SetPositionZ(positoinZ);
	}
}

// �J�����̃��[�r�[�X�V����
void G0_Onigokko::UpdateCameraMovie(float elapsedTime)
{

	switch (cameraMovieScene)
	{
	case CameraMovieScene::OniEntry:
	{
		cameraMovieTime += elapsedTime;

		std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

		target = oni->GetPosition();
		target.y = oni->GetHeight() * 0.8f;

		if (cameraMovieTime > 8.0f)
		{
			cameraMovieTime = 0;
			movieScene = false;
			cameraMovieScene = CameraMovieScene::OniMove;
		}
	}
	break;
	case CameraMovieScene::OniMove:
	{
		if (movieScene)
		{
			if (!movieFade)
			{
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					1.2f, 0.2f);

				movieFade = true;
			}

			movieWaitTimer -= elapsedTime;
			if (movieWaitTimer <= 0.0f)
				movieScene = false;
		}
		else
		{
			movieWaitTimer = 2;

			if (movieFade && !fade->GetFade())
			{
				if (player->GetHp() <= 0)
				{
					std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());
					// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
					SceneManager::Instance().ChangeScene(std::move(loadingScene));
				}

				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					1.0f, 0.0f,
					1.2f, 0.8f);

				movieFade = false;
			}
		}

		if (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() == 0)
		{
			movieScene = true;
			player->SetMovieTime(100);

			if (!nextOnideathFade)
			{
				fade->SetFadeUnlock();
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					1.0f, 0.2f);
				nextOnideathFade = true;
			}

			if(!fade->GetFade())
				cameraMovieScene = CameraMovieScene::OniDeath;
		}
	}
	break;
	case CameraMovieScene::OniDeath:
	{
		cameraMovieTime += elapsedTime;

		if (!oniDeathFade && !fade->GetFade())
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				1.0f, 0.0f,
				1.0f, 0.2f);

			oniDeathFade = true;
		}

		std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

		cameraController->SetRange(30);
		cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(10), 0, 0));
		target = oni->GetPosition();
		target.y = oni->GetHeight() * 0.8f;

		if (cameraMovieTime > 4.0f)
		{
			if (!gameFinishFade)
			{
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					1.0f, 0.2f);

				gameFinishFade = true;
			}
			else if (!fade->GetFade())
			{
				std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());
				// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
				SceneManager::Instance().ChangeScene(std::move(loadingScene));
			}
		}
	}
	break;
	default:
		break;
	}
}