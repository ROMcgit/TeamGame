#include "Graphics/Graphics.h"
#include "G1_DarumasangaKoronda.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyDarumasangaKoronda.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G1_StageDarumasangaKoronda.h"
#include "Game/Stage/G1_StageBumpiness.h"
#include "Game/Stage/G1_StageEndHole.h"
#include "Game/Stage/StageMoveFloor.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "G1_DarumasangaKoronda_Clear.h"
#include "G1_DarumasangaKoronda_GameOver.h"
#include "Audio/BgmManager.h"
#include <algorithm>

//! ���[�r�[����
bool G1_DarumasangaKoronda::movieScene = false;

// ������
void G1_DarumasangaKoronda::Initialize()
{
	movieScene = true;

	Graphics& graphics = Graphics::Instance();
	graphics.GetEnvironmentMap()->Load("Data/Environment/DemonsWorld.hdr");
	graphics.GetEnvironmentMap()->Set(15);

	//! ��
	sky = std::make_unique<Sky>();

	ID3D11Device* device = graphics.GetDevice();
	float screenWidth = graphics.GetScreenWidth();
	float screenHeight = graphics.GetScreenHeight();

	// �����_�[�^�[�Q�b�g
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// �V���h�E�}�b�v������
	shadowMap.Initialize();

/********************************************************************************/

	float posZ = 54.0f;
	// �X�e�[�W������
	for(int i = 0; i < 5; i ++)
	{
		float posZ = 54.0f + (i * 110.0f);

		StageManager& stageManager = StageManager::Instance();

		switch (i + 1)
		{
		case 1:
		case 2:
		{
			
			std::unique_ptr<G1_StageDarumasangaKoronda> stageMain = std::make_unique<G1_StageDarumasangaKoronda>();

			stageMain->SetPosition(DirectX::XMFLOAT3(0, 110.0f, posZ));
			stageManager.Register(std::move(stageMain));
		}
		break;
		case 3:
		{
			std::unique_ptr<G1_StageBumpiness> stageMain = std::make_unique<G1_StageBumpiness>();

			stageMain->SetPosition(DirectX::XMFLOAT3(0, 110.0f, posZ));
			stageManager.Register(std::move(stageMain));
		}
		break;
		case 4:
		{
			std::unique_ptr<G1_StageEndHole> stageMain = std::make_unique<G1_StageEndHole>();

			stageMain->SetPosition(DirectX::XMFLOAT3(0, 110.0f, posZ));
			stageManager.Register(std::move(stageMain));
		}
		break;
		case 5:
		{
			std::unique_ptr<G1_StageBumpiness> stageMain = std::make_unique<G1_StageBumpiness>();

			stageMain->SetPosition(DirectX::XMFLOAT3(0, 110.0f, posZ));
			stageManager.Register(std::move(stageMain));
		}
		break;
		default:
			break;
		}
		
	}

	// �v���C���[������
	player = std::make_unique<Player1_DarumasangaKoronda>();
	player->SetPosition(DirectX::XMFLOAT3(0, 113.0f, 0));

	//! �G
	std::unique_ptr<EnemyDarumasangaKoronda> oni = std::make_unique<EnemyDarumasangaKoronda>();
	oni->SetPosition(DirectX::XMFLOAT3(0.0f, 113.227f, 538.45f));
	//! �S��o�^
	EnemyManager::Instance().Register(std::move(oni));

	// �J���������ݒ�
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

	actionExplanation = std::make_unique<Sprite>("Data/Sprite/1.DarumasangaKoronda/ActionExplanation.png");

	//! �^�C�}�[
	timer = std::make_unique<Timer>(true, 2, 30);

	//! �|�[�Y
	pause = std::make_unique<Pause>();

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("����܂��񂪓]��", "Data/Audio/Bgm/8.Daruma.wav");
	bgm.PlayBgm("����܂��񂪓]��", 1.0f);
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
	//! �^�C�}�[�̍X�V����
	if (!movieScene)
	{
		if(pause->GetPauseOpacity() <= 0.0f)
			timer->Update(elapsedTime);

		pause->Update(elapsedTime);

		if (player->GetPosition().x < -6.0f && actionExplanationOpacity > 0.0f)
			actionExplanationOpacity -= 2 * elapsedTime;
		else if(actionExplanationOpacity < 1.0f)
			actionExplanationOpacity += 2 * elapsedTime;

		actionExplanationOpacity = std::clamp(actionExplanationOpacity, 0.3f, 1.0f);
	}

	if (pause->GetPauseOpacity() <= 0.0f)
	{
		//! �t�F�[�h���X�V����
		fade->Update(elapsedTime);

		//! ���[�r�[�X�V����
		UpdateMovie(elapsedTime);

		// �J�����R���g���[���[�X�V����
		if (!movieScene || (movieStep == 1 || movieStep == 2))
		{
			DirectX::XMFLOAT3 target = player->GetPosition();
			target.x = 0;
			target.y += player->GetHeight() * 0.5f;
			target.z -= 2.0f;
			if (target.z < 0.8f || target.z > 540.0f)
			{
				target.z = std::clamp(target.z, 0.8f, 540.0f);
			}
			cameraController->SetTarget(target);
			cameraController->SetRange(20.0f);
		}
		else
		{
			std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

			DirectX::XMFLOAT3 target = oni->GetPosition();
			target.x = 0;
			target.y += oni->GetHeight() * 0.5f;
			target.z -= 2.0f;
			cameraController->SetTarget(target);
			cameraController->SetRange(15.0f);
		}
		Camera::Instance().Update(elapsedTime);
		cameraController->Update(elapsedTime);

		// �X�e�[�W�X�V����
		StageManager::Instance().Update(elapsedTime);

		PlayerPositionControll();
		// �v���C���[�X�V����
		if (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 0))
			player->Update(elapsedTime);

		// �G�l�~�[�X�V����
		EnemyManager::Instance().Update(elapsedTime);

		// �G�t�F�N�g�X�V����
		EffectManager::Instance().Update(elapsedTime);

		// �V�[���؂�ւ�����
		SceneChange();
	}
}

// �`�揈��
void G1_DarumasangaKoronda::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 150.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 4.0f, 17.0f, 9.0f };

	vignetteOpacity = 0.8f;

	//! �t�H�O
	fogStart = 50.0f;
	fogEnd   = 100.0f;

	Graphics& graphics = Graphics::Instance();

	DrawingSettings(graphics);

	//! �V���h�E�}�b�v
	{
		// �|�[�Y��ʂ���Ȃ��Ȃ�
		if (pause->GetPauseOpacity() < 1.0f)
		{
			//! �V���h�E�}�b�v�J�n
			shadowMap.Begin(rc);
			{
				Shader* shadowMapShader = graphics.GetShadowMapShader();
				shadowMapShader->Begin(dc, rc);

				//�G�l�~�[�`��
				std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

				if(oni->GetOpacity() > 0.5f)
					EnemyManager::Instance().Render(dc, shadowMapShader);
				// �v���C���[�`��
				if (!movieScene)
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
		// �|�[�Y��ʂ���Ȃ��Ȃ�
		if (pause->GetPauseOpacity() < 1.0f)
		{
			//! �X�J�C�}�b�v
			{
				Shader* skyShader = graphics.GetSkydomeShader();
				skyShader->Begin(dc, rc);

				sky->Render(dc, skyShader);

				skyShader->End(dc);
			}

			Shader* shader = graphics.GetDefaultLitShader();
			shader->Begin(dc, rc);
			// �X�e�[�W�`��
			StageManager::Instance().Render(dc, shader);

			// �v���C���[�`��
			if (!movieScene)
				player->Render(dc, shader);

			// �J�����̈ʒu��`��
			cameraController->RenderCameraTarget(dc, shader);

			//�G�l�~�[�`��
			EnemyManager::Instance().Render(dc, shader);
			shader->End(dc);
		}
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
		//! �^�C�}�[
		DirectX::XMFLOAT3 color;
		if (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 30))
			color = { 1, 1, 1 };
		else
			color = { 1, 0, 0 };

		if(!movieScene)
			timer->Render(dc, graphics, DirectX::XMFLOAT2(30.0f, 0.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f));

		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(actionExplanation->GetTextureWidth());
		float textureHeight = static_cast<float>(actionExplanation->GetTextureHeight());

		/*if (!movieScene)
			actionExplanation->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, actionExplanationOpacity);*/

		//! �t�F�[�h�̕`�揈��
		fade->Render(dc, graphics);

		//if (!movieScene)
		//	//! �|�[�Y���
		//	pause->Render(dc, graphics);
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
void G1_DarumasangaKoronda::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	player->SetMovieTime(0.1f);

	switch (movieStep)
	{
	case 0:

		movieTime += elapsedTime;

		if (movieTime > 3.5f)
		{
			movieScene = false;
			movieTime = 0.0f;
			movieStep++;
		}

		break;
	//! �G���S
	case 1:

		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			1.0f, 0.2f);

		movieStep++;

		break;
	//! �t�F�[�h���I���
	case 2:

		if (!fade->GetFade())
		{
			movieTime += elapsedTime;

			if (movieTime > 1.0f)
			{
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					1.0f, 0.0f,
					1.0f, 0.2f);

				movieStep++;
			}
		}
		break;
	case 3:

		if (!fade->GetFade())
		{
			movieTime += elapsedTime;

			if (movieTime > 3.5f)
			{
				sceneChange = true;
			}
		}

		break;
	default:
		break;
	}
}

// �V�[���؂�ւ�����
void G1_DarumasangaKoronda::SceneChange()
{
	if (!sceneChange && (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 0))) return;

	if (!setFade)
	{
		//! �t�F�[�h��ݒ�
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			1.0f, 0.5f);

		setFade = true;
	}
	else if (!fade->GetFade())
	{
		BgmManager::Instance().UnloadBgm("����܂��񂪓]��");

		std::unique_ptr<SceneLoading> loadingScene;
		if((timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 0)))
			loadingScene = std::make_unique<SceneLoading>(std::make_unique<G1_DarumasangaKoronda_Clear>());
		else
			loadingScene = std::make_unique<SceneLoading>(std::make_unique<G1_DarumasangaKoronda_GameOver>());

		// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// �v���C���[�̈ʒu����
void G1_DarumasangaKoronda::PlayerPositionControll()
{
	float setPosX = 16.3f;
	if (player->GetPosition().x < -setPosX || player->GetPosition().x > setPosX)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -setPosX, setPosX);
		player->SetPositionX(positoinX);
	}

	float setPosZ = 287.0f;
	if (player->GetPosition().z < -2.0f || player->GetPosition().z > 545.0f)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -2.0f, 545.0f);
		player->SetPositionZ(positoinZ);
	}
}