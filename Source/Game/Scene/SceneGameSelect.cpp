#include "Graphics/Graphics.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneManager.h"

#include "Game/Scene/G0_Onigokko.h"
#include "Game/Scene/G1_DarumasangaKoronda.h"
#include "Game/Scene/G2_Sundome.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Game/Scene/G4_OssanTataki.h"
#include "Game/Scene/G5_Asibawatari.h"

#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/StageGameSelect.h"
#include "Game/Stage/StageMoveFloor.h"

#include "Game/Character/GameSelect/GameSelectManager.h"
#include "Game/Character/GameSelect/GS0_Onigokko.h"
#include "Game/Character/GameSelect/GS1_DarumasangaKoronda.h"
#include "Game/Character/GameSelect/GS2_Sundome.h"
#include "Game/Character/GameSelect/GS3_SoratobuHusenWari.h"
#include "Game/Character/GameSelect/GS4_OssanTataki.h"
#include "Game/Character/GameSelect/GS5_AsibaWatari.h"

#include <algorithm>

SceneGameSelect::GameSelect SceneGameSelect::gameSelect = SceneGameSelect::GameSelect::Onigokko;
bool SceneGameSelect::sceneChange = false;

// ������
void SceneGameSelect::Initialize()
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
	std::unique_ptr<StageGameSelect> stageMain = std::make_unique<StageGameSelect>();
	stageManager.Register(std::move(stageMain));

	// �v���C���[������
	player = std::make_unique<Player0_Onigokko>();
	player->SetPosition(DirectX::XMFLOAT3(0, -5.2f, 0));
	player->SetScale(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));
	player->SetMoveSpeed(20.0f);
	player->SetRadius(1.9f);
	player->SetHeight(16.0f);

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
	cameraController->SetRange(85.5f);
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(40), 0, 0));

	// �w�i
	backGround = std::make_unique<Sprite>();

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);

	sceneChange = false;

	GameSelectManager& gameSelectManager = GameSelectManager::Instance();

	float posX = 65.0f;

	//! ���ɂ�����
	std::unique_ptr<GS0_OniGokko> onigokko = std::make_unique<GS0_OniGokko>();
	onigokko->SetPosition(DirectX::XMFLOAT3(posX * -2.5f, 1, 80));
	gameSelectManager.Register(std::move(onigokko));

	//! ����܂��񂪓]��
	std::unique_ptr<GS1_DarumasangaKoronda> darumasangaKoronda = std::make_unique<GS1_DarumasangaKoronda>();
	darumasangaKoronda->SetPosition(DirectX::XMFLOAT3(posX * -1.5f, 1, 80));
	gameSelectManager.Register(std::move(darumasangaKoronda));

	//! ���~��
	std::unique_ptr<GS2_Sundome> sundome = std::make_unique<GS2_Sundome>();
	sundome->SetPosition(DirectX::XMFLOAT3(posX * -0.5f, 1, 80));
	gameSelectManager.Register(std::move(sundome));

	//! ���ԕ��D����
	std::unique_ptr<GS3_SoratobuHusenWari> soratobuHusenWari = std::make_unique<GS3_SoratobuHusenWari>();
	soratobuHusenWari->SetPosition(DirectX::XMFLOAT3(posX * 0.5f, 1, 80));
	gameSelectManager.Register(std::move(soratobuHusenWari));

	//! ��������@��
	std::unique_ptr<GS4_OssanTataki> ossanTataki = std::make_unique<GS4_OssanTataki>();
	ossanTataki->SetPosition(DirectX::XMFLOAT3(posX * 1.5f, 1, 80));
	gameSelectManager.Register(std::move(ossanTataki));

	//! ����n��
	std::unique_ptr<GS5_AsibaWatari> asibaWatari = std::make_unique<GS5_AsibaWatari>();
	asibaWatari->SetPosition(DirectX::XMFLOAT3(posX * 2.5f, 1, 80));
	gameSelectManager.Register(std::move(asibaWatari));
}

// �I����
void SceneGameSelect::Finalize()
{
	//! �X�e�[�W��j������
	StageManager::Instance().Clear();

	//! �G��S�Ĕj������
	EnemyManager::Instance().Clear();

	//! �G�t�F�N�g��S�Ĕj������
	EffectManager::Instance().Cleanup();
}

// �X�V����
void SceneGameSelect::Update(float elapsedTime)
{
	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y = player->GetHeight() * 0.5f;

	if (target.x < -126.0f || target.x > 126.0f)
	{
		target.x = std::clamp(target.x, -126.0f, 126.0f);
	}

	if (target.z < -35.0f || target.z > 35.0f)
	{
		target.z = std::clamp(target.z, -35.0f, 35.0f);
	}

	cameraController->SetTarget(target);
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);

	// �v���C���[�̈ʒu����
	PlayerPositionControll();
	// �v���C���[�X�V����
	player->Update(elapsedTime);

	fade->Update(elapsedTime);

	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);

	GameSelectManager::Instance().Update(elapsedTime);

	if (sceneChange)
	{
		player->SetMovieTime(1.0f);

		if (!setFade)
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				1.0f, 0.2f);

			setFade = true;
		}

		if (setFade && !fade->GetFade())
		{
			std::unique_ptr<SceneLoading> loadingScene;

			switch (gameSelect)
			{
			case GameSelect::Onigokko:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G0_Onigokko>());
				break;
			case GameSelect::DarumasangaKoronda:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G1_DarumasangaKoronda>());
				break;
			case GameSelect::Sundome:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G2_Sundome>());
				break;
			case GameSelect::SoratobuHusenWari:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G3_SoratobuHusenWari>());
				break;
			case GameSelect::OssanTataki:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G4_OssanTataki>());
				break;
			case GameSelect::Asibawatari:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G5_Asibawatari>());
				break;
			default:
				break;
			}

			// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}

// �`�揈��
void SceneGameSelect::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 5.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 3.0f, 22.0f, 9.0f };

	//! �t�H�O
	fogStart = 100.0f;
	fogEnd   = 150.0f;

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

		GameSelectManager::Instance().Render(dc, shader);

		// �v���C���[�`��
		player->Render(dc, shader);

		//�G�l�~�[�`��
		EnemyManager::Instance().Render(dc,shader);
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

		GameSelectManager::Instance().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}
#endif // DEBUG

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
		fade->Render(dc, graphics);
	}
	
#ifndef _DEBUG

	// 2D�f�o�b�OGUI�`��
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::Image(shadowMap.GetSRV(), ImVec2(300, 200));
			StageManager::Instance().DrawDebugGUI();

			GameSelectManager::Instance().DrawDebugGUI();

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
#endif
}

// �v���C���[�̈ʒu����
void SceneGameSelect::PlayerPositionControll()
{
	float posX = 185.0f;

	if (player->GetPosition().x < -posX || player->GetPosition().x > posX)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -posX, posX);
		player->SetPositionX(positoinX);
	}

	if (player->GetPosition().z < -61.2 || player->GetPosition().z > 130.0f)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -61.2f, 130.0f);
		player->SetPositionZ(positoinZ);
	}
}
