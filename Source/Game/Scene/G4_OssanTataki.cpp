#include "Graphics/Graphics.h"
#include "G4_OssanTataki.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"

#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G4_StageOssanTataki.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Hole.h"
#include "Game/Character/Enemy/EnemyChara.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "G4_OssanTataki_Result.h"
#include "Audio/BgmManager.h"

//! �X�R�A
int G4_OssanTataki::score = 0;

// ������
void G4_OssanTataki::Initialize()
{
	score = 0;

	movieScene = true;

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

	//! �|�X�g�G�t�F�N�g�̐ݒ�(��񂾂�)�t���O��������
	camera.SetPostEffectStatusOnceUnlock();

	//�J�����R���g���[���[������
	cameraController = std::make_unique <CameraController>();
	cameraController->SetTarget(DirectX::XMFLOAT3(10, 56, -44.0f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(49), 0, 0));
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

	//! �^�C�}�[
	timer = std::make_unique<Timer>(true, 1, 30);

	//! �|�[�Y���
	pause = std::make_unique<Pause>();

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("��������@��", "Data/Audio/Bgm/11.Ossantataki.wav");
	bgm.PlayBgm("��������@��", 0.3f);
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
	//! �|�[�Y���
	pause->Update(elapsedTime);

	if(pause->GetPauseOpacity() <= 0.0f)
	{
		//! �t�F�[�h�̍X�V����
		fade->Update(elapsedTime);

		//! �^�C�}�[�̍X�V����
		if (!movieScene)
			timer->Update(elapsedTime);

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
		if (!movieScene)
			player->Update(elapsedTime);

		// �G�l�~�[�X�V����
		if (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 0))
			EnemyManager::Instance().Update(elapsedTime);

		// �G�t�F�N�g�X�V����
		EffectManager::Instance().Update(elapsedTime);

		// �G��������
		NewEnemy(elapsedTime);

		// �X�R�A�X�V����
		UpdateScore();

		// �V�[���؂�ւ�����
		SceneChange();
	}
}

// �`�揈��
void G4_OssanTataki::Render()
{
	lightPosition.x = 2.8f;
	lightPosition.y = 43.5f;
	lightPosition.z = -43.8f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 0.7f, 23.0f, 6.0f };

	//! �t�H�O
	fogStart = 10.0f;
	fogEnd   = 380.0f;
	fogColor = { 0, 0.9f, 1.0f };

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
				//EnemyManager::Instance().Render(dc, shadowMapShader);
				// �v���C���[�`��
				//player->Render(dc, shadowMapShader);

				// �ՓˍU���̕`�揈��
				//collisionAttackManager.Render(dc, shadowMapShader);

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
			shader->End(dc);
		}
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
		//! �X�R�A�̉摜
		scoreText->RenderOku(dc, false, score, false,
			scoreTextPos.x, scoreTextPos.y);

		//! �^�C�}�[
		DirectX::XMFLOAT3 color;
		if (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 30))
			color = { 1, 1, 1 };
		else
			color = { 1, 0, 0 };

		timer->Render(dc, graphics, DirectX::XMFLOAT2(30.0f, 0.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f));

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

			ImGui::DragInt("Score", &score);

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
		}
		ImGui::End();
	}
#endif // !_DEBUG
}

// ���[�r�[�X�V����
void G4_OssanTataki::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	switch (movieStep)
	{
	case 0:

		movieTime += elapsedTime;

		if (movieTime > 6.0f)
		{
			movieScene = false;
		}

		break;
	}
}

// �G��������
void G4_OssanTataki::NewEnemy(float elapsedTime)
{
	if (newEnemy) return;

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

#if 1
	for(int i = 0; i < 4; i++)
	{
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(i);

		// �ʒu
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyChara> chara = std::make_unique<EnemyChara>();
		chara->SetPosition(pos);

		//! ���������o�^
		EnemyManager::Instance().Register(std::move(chara));
	}
#endif

	newEnemy = true;
}

// �X�R�A�X�V����
void G4_OssanTataki::UpdateScore()
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

// �V�[���؂�ւ�����
void G4_OssanTataki::SceneChange()
{
	if (timer->GetTimeM_Int() <= 0 && timer->GetTimeS_Int() <= 0)
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
			BgmManager::Instance().UnloadBgm("��������@��");

			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G4_OssanTataki_Result>());

			// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}