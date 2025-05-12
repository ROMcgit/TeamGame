#include "Graphics/Graphics.h"
#include "G3_SoratobuHusenWari.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G3_StageSoratobuHusenWari.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Character/Item/Balloon_Plus.h"
#include "Game/Character/Item/Balloon_Minus.h"
#include "Game/Character/Item/ItemManager.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Cloud.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "G3_SoratobuHusenWari_Result.h"

//! ���[�r�[����
bool G3_SoratobuHusenWari::movieScene = false;

//! �X�R�A
int G3_SoratobuHusenWari::score = 0;

// ������
void G3_SoratobuHusenWari::Initialize()
{
	//! �X�R�A
	score = 0;

	//! �X�R�A�̉摜
	scoreText = std::make_unique<Text>();

	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// �����_�[�^�[�Q�b�g
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// �V���h�E�}�b�v������
	shadowMap.Initialize();

/********************************************************************************/

	// �X�e�[�W������
	for(int i = 0; i < 3; i++)
	{
		StageManager& stageManager = StageManager::Instance();
		std::unique_ptr<G3_StageSoratobuHusenWari> stageMain = std::make_unique<G3_StageSoratobuHusenWari>();
	
		float posZ = 0.0f;
		if (i == 0)
		{
			posZ = 50.0f;
		}
		else if(i == 1)
		{
			posZ = 160.0f;
		}
		else
		{
			posZ = 270.0f;
		}

		stageMain->SetPositionZ(posZ);

		stageManager.Register(std::move(stageMain));
	}

	// �v���C���[������
	player = std::make_unique<Player3_SoratobuHusenWari>();
	player->SetPosition(DirectX::XMFLOAT3(0, 20, 0));

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
	cameraController->SetTarget(DirectX::XMFLOAT3(0, 21.0f, 4.8f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(3), 0, 0));
	cameraController->SetRange(21.0f);

	// �w�i
	backGround = std::make_unique<Sprite>();

	fade = std::make_unique<Fade>();
	//! �t�F�[�h��ݒ�
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);
}

// �I����
void G3_SoratobuHusenWari::Finalize()
{
	//! �X�e�[�W��j������
	StageManager::Instance().Clear();

	//! �G��S�Ĕj������
	EnemyManager::Instance().Clear();

	//! �G�t�F�N�g��S�Ĕj������
	EffectManager::Instance().Cleanup();
}

// �X�V����
void G3_SoratobuHusenWari::Update(float elapsedTime)
{
	//! �t�F�[�h�̍X�V����
	fade->Update(elapsedTime);

	//! �^�[�Q�b�g��ݒ�
	cameraController->SetTarget(DirectX::XMFLOAT3(0, 21.0f, 4.8f));
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);

	// �v���C���[�X�V����
	player->Update(elapsedTime);

	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �A�C�e���̍X�V����
	ItemManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);

	// �ՓˍU���̍X�V����
	collisionAttackManager.Update(elapsedTime);

	//! �o���[����������
	NewBalloon(elapsedTime);

	//! �_��������
	NewCloud(elapsedTime);

	//! �X�R�A�X�V����
	UpdateScore();

	//! �V�[���؂�ւ�
	SceneChange();
}

// �`�揈��
void G3_SoratobuHusenWari::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 5.0f;
	lightPosition.z = CameraController::target.z + 20.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 0.0f, 55.0f, 5.5f };

	//! �t�H�O
	fogStart = 30.0f;
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

				// �A�C�e���`�揈��
				ItemManager::Instance().Render(dc, shadowMapShader);
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

		// �A�C�e���`�揈��
		ItemManager::Instance().Render(dc, shader);

		// �v���C���[�`��
		player->Render(dc, shader);

		// �ՓˍU���̕`��
		collisionAttackManager.Render(dc, shader);

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

		// �A�C�e��
		ItemManager::Instance().DrawDebugPrimitive();

		// �ՓˍU��
		collisionAttackManager.DrawDebugPrimitive();

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
		//! �X�R�A�̉摜
		scoreText->RenderOku(dc, false, score, false,
			scoreTextPos.x, scoreTextPos.y);

		//! �v���C���[�̃X�v���C�g�`��
		player->SpriteRender(dc);

		//! �t�F�[�h�̕`�揈��
		fade->Render(dc, graphics);
	}

	// 2D�f�o�b�OGUI�`��
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			//! �X�R�A�̈ʒu
			ImGui::DragFloat2("ScorePos", &scoreTextPos.x);
			//! �X�R�A
			ImGui::DragInt("Score", &score);

			collisionAttackManager.DrawDebugGUI();
			ItemManager::Instance().DrawDebugGUI();

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

// ���D��������
void G3_SoratobuHusenWari::NewBalloon(float elapsedTime)
{
	if (newBalloonWaitTime > 0.0f)
	{
		newBalloonWaitTime -= elapsedTime;
		return;
	}

	ItemManager& itemManager = ItemManager::Instance();
	int ItemCount = itemManager.GetItemCount();

	if (ItemCount < 10)
	{
		int itemRansu = rand() % 2 + 1;

		DirectX::XMFLOAT3 pos;
		pos.x = rand() % 11 * (rand() % 2 == 1 ? -1 : 1);
		pos.y = rand() % 8 + 17.0f;
		pos.z = 100.0f;

		switch (itemRansu)
		{
		case 1:
		{
			std::unique_ptr<Balloon_Plus> balloon_Plus = std::make_unique<Balloon_Plus>();

			balloon_Plus->SetPosition(pos);
			balloon_Plus->SetPositionResetY(pos.y);

			itemManager.Register(std::move(balloon_Plus));
		}
			break;
		case 2:
		{
			std::unique_ptr<Balloon_Minus> balloon_Minus = std::make_unique<Balloon_Minus>();

			balloon_Minus->SetPosition(pos);
			balloon_Minus->SetPositionResetY(pos.y);

			itemManager.Register(std::move(balloon_Minus));
		}
			break;
		default:
			break;
		}
		newBalloonWaitTime = 1.3f;
	}
}

// �_��������
void G3_SoratobuHusenWari::NewCloud(float elapsedTime)
{
	gameTimer += elapsedTime;

	if (newCloudWaitTime > 0.0f)
	{
		newCloudWaitTime -= elapsedTime;
		return;
	}

	int collisionAttackCount = collisionAttackManager.GetCollisionAttackCount();
	int maxCollisionAttackCount = 0;

	if (gameTimer > 60.0f)
		maxCollisionAttackCount = 1;
	else if(gameTimer > 120.0f)
		maxCollisionAttackCount = 2;
	else if(gameTimer > 240.0f)
		maxCollisionAttackCount = 3;
	else if (gameTimer > 360.0f)
		maxCollisionAttackCount = 4;

	if (collisionAttackCount < maxCollisionAttackCount)
	{
		DirectX::XMFLOAT3 pos;
		pos.x = rand() % 13 * (rand() % 2 == 1 ? -1 : 1);
		pos.y = rand() % 8 + 18.5f;
		pos.z = 100.0f;

		//! �_
		std::unique_ptr<CollisionAttack_Cloud> cloud = std::make_unique<CollisionAttack_Cloud>(&collisionAttackManager);
		cloud->SetPosition(pos);

		collisionAttackManager.Register(std::move(cloud));

		newCloudWaitTime = 5.0f;
	}
}

// �X�R�A�X�V����
void G3_SoratobuHusenWari::UpdateScore()
{
	//! �X�R�A���ő�l�𒴂��Ȃ��悤�ɂ���
	if (score > scoreText->GetMaxOku())
	{
		score = scoreText->GetMaxOku();
	}
	else if (score < scoreText->GetMin())
	{
		score = scoreText->GetMin();
	}
}

// �V�[���ύX����
void G3_SoratobuHusenWari::SceneChange()
{
	if(player->GetHp() <= 0)
	{
		if (!setFade)
		{
			//! �t�F�[�h��ݒ�
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				1.0f, 0.5f);

			setFade = true;
		}
		else if(setFade && !fade->GetFade())
		{
			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G3_SoratobuHusenWari_Result>());

			// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}