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

/********************************************************************************/

	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	std::unique_ptr<StageGameSelect> stageMain = std::make_unique<StageGameSelect>();
	stageManager.Register(std::move(stageMain));

	// �v���C���[������
	player = std::make_unique<Player0_Onigokko>();
	player->SetPosition(DirectX::XMFLOAT3(0, -5.35f, 0));

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
	cameraController->SetRange(110);
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(40), 0, 0));

	// �w�i
	backGround = std::make_unique<Sprite>();

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);

	sceneChange = false;

	std::unique_ptr<GS0_OniGokko> onigokko = std::make_unique<GS0_OniGokko>();
	onigokko->SetPosition(DirectX::XMFLOAT3(0, 1, 80));
	GameSelectManager::Instance().Register(std::move(onigokko));
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
	target.y += 0.5f;
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
	lightPosition = CameraController::target;
	lightPosition.y += 0.5f;

	Graphics& graphics = Graphics::Instance();
	
	DrawingSettings(graphics);

	//! �����_�[�^�[�Q�b�g
	{
		//! �����_�[�^�[�Q�b�g�ɕ`��J�n
		renderTarget->Begin();
	}

	//! 2D�X�v���C�g
	{
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
	}

	// 3D���f���`��
	{
		Shader* shader = graphics.GetDefaltLitShader();
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

#ifdef _DEBUG

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
		Camera::Instance().SetPostEffectStatusOnce(
			1.0f, 0.8f,
			DirectX::XMFLOAT3(1.2f, 1.3f, 1.35f), 0);
		//! �X�N���[�����|�X�g�G�t�F�N�g�V�F�[�_�[�ŕ`��
		renderTarget->Render();
	}

	{
		fade->Render(dc, graphics);
	}
	
#ifdef _DEBUG

	// 2D�f�o�b�OGUI�`��
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
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
	if (player->GetPosition().x < -300.0f || player->GetPosition().x > 300.0f)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -300.0f, 300.0f);
		player->SetPositionX(positoinX);
	}

	if (player->GetPosition().z < -300.0f || player->GetPosition().z > 300.0f)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -300.0f, 300.0f);
		player->SetPositionZ(positoinZ);
	}
}
