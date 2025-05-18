#include "Graphics/Graphics.h"
#include "G5_Asibawatari.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G5_StageAsibawatari_Normal.h"
#include "Game/Stage/G5_StageAsibawatari_Normal_Horizontal.h"
#include "Game/Stage/G5_StageAsibawatari_Normal_Vertical.h"
#include "Game/Stage/G5_StageAsibawatari_Trap.h"
#include "Game/Stage/G5_StageAsibawatari_Goal.h"
#include "Game/Stage/StageMoveFloor.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "G5_Asibawatari_GameOver.h"
#include "G5_Asibawatari_Clear.h"
#include "Audio/BgmManager.h"

bool G5_Asibawatari::gameClear = false;

// ������
void G5_Asibawatari::Initialize()
{
	gameClear = false;

	movieScene = true;

	Graphics& graphics = Graphics::Instance();
	graphics.GetEnvironmentMap()->Load("Data/Environment/SF_Night.hdr");
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

		// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	std::unique_ptr<G5_StageAsibawatari_Normal> stageMain = std::make_unique<G5_StageAsibawatari_Normal>();
	stageMain->SetMoveSpeed(0.0f);
	stageManager.Register(std::move(stageMain));

	// �v���C���[������
	player = std::make_unique<Player5_AsibaWatari>();
	player->SetPosition(DirectX::XMFLOAT3(0, 5.0f, 0));
	player->SetAngleY(DirectX::XMConvertToRadians(180));
	//player->SetGravity(0.0f);

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
	cameraController->SetTarget(DirectX::XMFLOAT3(0, 0.5f, 0.0f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(47), 0, 0));
	cameraController->SetRange(48.5f);

	// �w�i
	backGround = std::make_unique<Sprite>();

	//! �t�F�[�h
	fade = std::make_unique<Fade>();
	//! �t�F�[�h��ݒ�
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);

	//! �|�[�Y���
	pause = std::make_unique<Pause>();

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("����n��", "Data/Audio/Bgm/12.Asibawatari.wav");
	bgm.PlayBgm("����n��", 0.5f);
}

// �I����
void G5_Asibawatari::Finalize()
{
	//! �X�e�[�W��j������
	StageManager::Instance().Clear();

	//! �G��S�Ĕj������
	EnemyManager::Instance().Clear();

	//! �G�t�F�N�g��S�Ĕj������
	EffectManager::Instance().Cleanup();
}

// �X�V����
void G5_Asibawatari::Update(float elapsedTime)
{
	//! �|�[�Y��ʂ̍X�V����
	pause->Update(elapsedTime);

	if(pause->GetPauseOpacity() <= 0.0f)
	{
		//! �t�F�[�h�̍X�V����
		fade->Update(elapsedTime);

		//! ���[�r�[�X�V����
		UpdateMovie(elapsedTime);

		// �J�����R���g���[���[�X�V����
		Camera::Instance().Update(elapsedTime);
		cameraController->Update(elapsedTime);

		// �X�e�[�W�X�V����
		if (!gameClear)
			StageManager::Instance().Update(elapsedTime);

		//! �X�e�[�W��������
		NewStage(elapsedTime);

		// �v���C���[�X�V����
		if (!gameClear)
			player->Update(elapsedTime);

		// �G�l�~�[�X�V����
		EnemyManager::Instance().Update(elapsedTime);

		// �G�t�F�N�g�X�V����
		EffectManager::Instance().Update(elapsedTime);

		//! �V�[���؂�ւ�����
		SceneChange();
	}
}

// �`�揈��
void G5_Asibawatari::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 5.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 4.0f, 25.0f, 9.0f };

	//! �t�H�O
	fogStart = 2000.0f;
	fogEnd = 2100.0f;

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
				EnemyManager::Instance().Render(dc, shadowMapShader);
				// �v���C���[�`��
				player->Render(dc, shadowMapShader, true);

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

	{
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
void G5_Asibawatari::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	player->SetMovieTime(0.1f);

	switch (movieStep)
	{
	case 0:

		movieTime += elapsedTime;

		if (movieTime > 2.0f)
		{
			std::unique_ptr<Stage>& stage = StageManager::Instance().GetStage(0);
			stage->SetMoveSpeed(2.0f);

			movieScene = false;
		}

		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}

// �X�e�[�W��������
void G5_Asibawatari::NewStage(float elapsedTime)
{
	if (movieScene || newStageFinish) return;

	gameTimer += elapsedTime;

	float posX = 50.0f;

	//! �ʒuZ��-18�`18

	switch (stageStep)
	{
		//! �ʏ�
	case 0:

		if (gameTimer > 2.0f)
		{
			//! �ʏ�
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(normal));

			gameTimer = 0.0f;
			stageStep++;
		}
		break;
	case 1:

		if (gameTimer > 1.8f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					//! ����
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float posZ = i == 0 ? 14 : -14;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
		break;
	case 2:

		if (gameTimer > 10.0f)
		{
			//! �ʏ�
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);
			//normal->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(normal));

			//! �c��
			std::unique_ptr<G5_StageAsibawatari_Normal_Vertical> vertical = std::make_unique<G5_StageAsibawatari_Normal_Vertical>();
			vertical->SetPositionX(posX + 17);
			vertical->SetPositionZ(0);
			vertical->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(vertical));

			gameTimer = 0.0f;
			stageStep++;
		}
		break;
	case 3:
	{
		if (gameTimer > 6.3f)
		{
			//! �ʏ�
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(10);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.075f, 0.1f, 0.075f));

			StageManager::Instance().Register(std::move(normal));

			//! �g���b�v
			std::unique_ptr<G5_StageAsibawatari_Trap> trap = std::make_unique<G5_StageAsibawatari_Trap>();
			trap->SetPositionX(posX);
			trap->SetPositionZ(-10);
			trap->SetMoveSpeed(5.0f);
			trap->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(trap));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
		break;
	case 4:
	{
		if (gameTimer > 4.0f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					//! ����
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float num = j == 0 ? 12 : 6;
					float posZ = i == 0 ? num : -num;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 5:
	{
		if (gameTimer > 9.5f)
		{
			//! �ʏ�
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.075f, 0.1f, 0.075f));

			StageManager::Instance().Register(std::move(normal));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 6:
	{
		if (gameTimer > 4.0f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					//! ����
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float num = j == 0 ? 6 : 12;
					float posZ = i == 0 ? num : -num;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 7:
	{
		if (gameTimer > 8.0f)
		{
			//! �ʏ�
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX + 17);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(normal));

			//! �c��
			std::unique_ptr<G5_StageAsibawatari_Normal_Vertical> vertical = std::make_unique<G5_StageAsibawatari_Normal_Vertical>();
			vertical->SetPositionX(posX);
			vertical->SetPositionZ(0);
			vertical->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(vertical));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 8:
	{
		if (gameTimer > 8.0f)
		{
			//! �ʏ�
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(-10);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.075f, 0.1f, 0.075f));

			StageManager::Instance().Register(std::move(normal));

			//! �g���b�v
			std::unique_ptr<G5_StageAsibawatari_Trap> trap = std::make_unique<G5_StageAsibawatari_Trap>();
			trap->SetPositionX(posX);
			trap->SetPositionZ(10);
			trap->SetMoveSpeed(5.0f);
			trap->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(trap));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 9:
	{
		if (gameTimer > 4.0f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 5; j++)
					//! ����
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float num = 0;
					switch (j)
					{
					case 0: num = 12; break;
					case 1: num = 6; break;
					case 2: num = 10; break;
					case 3: num = 6; break;
					case 4: num = 12; break;
					default:
						break;
					}

					float posZ = i == 0 ? num : -num;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 10:
	{
		if (gameTimer > 25.0f)
		{
			//! �S�[��
			std::unique_ptr<G5_StageAsibawatari_Goal> goal = std::make_unique<G5_StageAsibawatari_Goal>();
			goal->SetPositionX(posX);
			goal->SetPositionZ(0);
			goal->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(goal));

			newStageFinish = true;
		}
	}
	break;
	default:
		break;
	}
}

// �V�[���؂�ւ�����
void G5_Asibawatari::SceneChange()
{
	//! �v���C���[���������A�܂��̓Q�[�����N���A�����Ȃ�
	if(player->GetPosition().y < -10.0f || gameClear)
	{
		if (!setFade)
		{
			//! �t�F�[�h��ݒ�
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				1.0f, 0.5f);

			setFade = true;
		}
		else if (setFade && !fade->GetFade())
		{
			BgmManager::Instance().UnloadBgm("����n��");

			std::unique_ptr<SceneLoading> loadingScene;
			if(player->GetPosition().y < -10.0f)
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G5_Asibawatari_GameOver>());
			else
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G5_Asibawatari_Clear>());

			// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}