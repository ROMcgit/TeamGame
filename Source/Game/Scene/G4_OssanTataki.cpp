#include "Graphics/Graphics.h"
#include "G4_OssanTataki.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyManager1.h"
#include "Game/Character/Enemy/EnemyManager2.h"
#include "Game/Character/Enemy/EnemyManager3.h"

#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G4_StageOssanTataki.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Hole.h"
#include "Game/Character/Enemy/EnemyOssan.h"
#include "Game/Character/Enemy/EnemyOssan1.h"
#include "Game/Character/Enemy/EnemyOssan2.h"
#include "Game/Character/Enemy/EnemyOssan3.h"

bool G4_OssanTataki::isEnemy[4] = { false, false, false, false };

// ������
void G4_OssanTataki::Initialize()
{
	for (int i = 0; i < 4; i++)
	{
		isEnemy[i] = false;
	}

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
	std::unique_ptr<G4_StageOssanTataki> stageMain = std::make_unique<G4_StageOssanTataki>();
	stageManager.Register(std::move(stageMain));

	// �v���C���[������
	player = std::make_unique<Player4_OssanTataki>();

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
	cameraController->SetTarget(DirectX::XMFLOAT3(10, 56, -39.0f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(63), 0, 0));
	cameraController->SetRange(56.0f);

	for (int i = 0; i < 4; i++)
	{
		// ��
		std::unique_ptr<CollisionAttack_Hole> hole = std::make_unique<CollisionAttack_Hole>(&collisionAttackManager);
		//! �ʒu
		DirectX::XMFLOAT3 pos = { 10.0f, 21.5f, -19.0f };

		float offsetX = 32.0f;
		float offsetZ = 17.5f;
		switch (i + 1)
		{
		case 1:
			pos.x -= offsetX;
			break;
		case 2:
			pos.x += offsetX;
			break;
		case 3:
			pos.z += offsetZ;
			break;
		case 4:
			pos.z -= offsetZ;
			break;
		default:
			break;
		}

		//! �ʒu��ݒ�
		hole->SetPosition(pos);

		collisionAttackManager.Register(std::move(hole));
	}

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
void G4_OssanTataki::Finalize()
{
	//! �X�e�[�W��j������
	StageManager::Instance().Clear();

	//! �G��S�Ĕj������
	EnemyManager::Instance().Clear();

	//! �G�t�F�N�g��S�Ĕj������
	EffectManager::Instance().Cleanup();
}

// �X�V����
void G4_OssanTataki::Update(float elapsedTime)
{
	//! �t�F�[�h�̍X�V����
	fade->Update(elapsedTime);

	//! ���[�r�[�X�V����
	UpdateMovie(elapsedTime);

	// �ՓˍU���̍X�V����
	collisionAttackManager.Update(elapsedTime);

	// �J�����R���g���[���[�X�V����
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);

	// �v���C���[�X�V����
	player->Update(elapsedTime);

	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);
	EnemyManager1::Instance().Update(elapsedTime);
	EnemyManager2::Instance().Update(elapsedTime);
	EnemyManager3::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);

	// �G��������
	NewEnemy(elapsedTime);
}

// �`�揈��
void G4_OssanTataki::Render()
{
	lightPosition.x = 2.8f;
	lightPosition.y = 43.5f;
	lightPosition.z = -43.8f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 0.7f, 23.0f, 6.0f };

	shadowMapBias = 0.015f;

	//! �t�H�O
	fogStart = 2000.0f;
	fogEnd = 2100.0f;

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
				EnemyManager1::Instance().Render(dc, shadowMapShader);
				EnemyManager2::Instance().Render(dc, shadowMapShader);
				EnemyManager3::Instance().Render(dc, shadowMapShader);
				// �v���C���[�`��
				player->Render(dc, shadowMapShader);

				// �ՓˍU���̕`�揈��
				collisionAttackManager.Render(dc, shadowMapShader);

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

		cameraController->RenderCameraTarget(dc, shader);

		// �ՓˍU���̕`�揈��
		collisionAttackManager.Render(dc, shader);

		//�G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);
		EnemyManager1::Instance().Render(dc, shader);
		EnemyManager2::Instance().Render(dc, shader);
		EnemyManager3::Instance().Render(dc, shader);
		shader->End(dc);

	}

	// 3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		// �v���C���[�f�o�b�O�v���~�e�B�u�`��
		player->DrawDebugPrimitive();

		// �G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		EnemyManager::Instance().DrawDebugPrimitive();
		EnemyManager1::Instance().DrawDebugPrimitive();
		EnemyManager2::Instance().DrawDebugPrimitive();
		EnemyManager3::Instance().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
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

	{
		//! �t�F�[�h�̕`�揈��
		fade->Render(dc, graphics);
	}

	// 2D�f�o�b�OGUI�`��
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			collisionAttackManager.DrawDebugGUI();
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
			EnemyManager1::Instance().DrawDebugGUI();
			EnemyManager2::Instance().DrawDebugGUI();
			EnemyManager3::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}
}

// ���[�r�[�X�V����
void G4_OssanTataki::UpdateMovie(float elapsedTime)
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

// �G��������
void G4_OssanTataki::NewEnemy(float elapsedTime)
{
	for (int i = 0; i < 4; i++)
	{
		if (newEnemyWaitTime[i] > 0.0f)
			newEnemyWaitTime[i] -= elapsedTime;
	}

	EnemyManager& enemyManager0 = EnemyManager::Instance();
	int enemyCount0 = enemyManager0.GetEnemyCount();

	if (enemyCount0 < 1 && newEnemyWaitTime[0] <= 0.0f)
	{
#if 1
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(0);

		// �ʒu
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan> ossan = std::make_unique<EnemyOssan>();
		ossan->SetPosition(pos);

		//! ���������o�^
		EnemyManager::Instance().Register(std::move(ossan));

		newEnemyWaitTime[0] = 1.0f;
#endif
	}

	EnemyManager1& enemyManager1 = EnemyManager1::Instance();
	int enemyCount1 = enemyManager1.GetEnemyCount();

	if (enemyCount1 < 1 && newEnemyWaitTime[1] <= 0.0f)
	{
#if 1
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(1);

		// �ʒu
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan1> ossan = std::make_unique<EnemyOssan1>();
		ossan->SetPosition(pos);

		//! ���������o�^
		EnemyManager1::Instance().Register(std::move(ossan));

		newEnemyWaitTime[1] = 1.0f;
#endif
	}

	EnemyManager2& enemyManager2 = EnemyManager2::Instance();
	int enemyCount2 = enemyManager2.GetEnemyCount();

	if (enemyCount2 < 1 && newEnemyWaitTime[2] <= 0.0f)
	{
#if 1
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(2);

		// �ʒu
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan2> ossan = std::make_unique<EnemyOssan2>();
		ossan->SetPosition(pos);

		//! ���������o�^
		EnemyManager2::Instance().Register(std::move(ossan));

		newEnemyWaitTime[2] = 1.0f;
#endif
	}

	EnemyManager3& enemyManager3 = EnemyManager3::Instance();
	int enemyCount3 = enemyManager3.GetEnemyCount();

	if (enemyCount3 < 1 && newEnemyWaitTime[3] <= 0.0f)
	{
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(3);

		// �ʒu
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan3> ossan = std::make_unique<EnemyOssan3>();
		ossan->SetPosition(pos);

		//! ���������o�^
		EnemyManager3::Instance().Register(std::move(ossan));

		newEnemyWaitTime[3] = 1.0f;
	}
}