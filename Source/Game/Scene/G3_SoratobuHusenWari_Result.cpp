#include "Graphics/Graphics.h"
#include "Game/Scene/G3_SoratobuHusenWari_Result.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "SceneLoading.h"
#include "Input/Input.h"
#include "Graphics/Fade.h"
#include "Other/Easing.h"

// ������
void G3_SoratobuHusenWari_Result::Initialize()
{
	// ���U���g
	result = std::make_unique<Sprite>("Data/Sprite/Result.png");

	// �q���g
	hint = std::make_unique<Sprite>("Data/Sprite/3.SoratobuHusenWari/Hint.png");

	// �X�R�A
	score = std::make_unique<Text>();

	float screenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth());
	float screenHeight = static_cast<float>(Graphics::Instance().GetScreenHeight());

	scorePos = { screenWidth * 2.0f, 273.0f };

	startScorePosX = scorePos.x;

	scoreDirector = true;

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);

	if (G3_SoratobuHusenWari::score >= 100)
		SceneGameSelect::clear.soratobuHusenWari = true;

	std::string filePath = "";
	if (SceneGameSelect::clear.soratobuHusenWari)
		filePath = "Data/Sprite/3.SoratobuHusenWari/Bonus.png";
	else
		filePath = "Data/Sprite/GameSelect/3.png";

	// �w�i
	backGround = std::make_unique<Sprite>(filePath.c_str());
}

// �I����
void G3_SoratobuHusenWari_Result::Finalize()
{
}

// �X�V����
void G3_SoratobuHusenWari_Result::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	fade->Update(elapsedTime);

	//! �X�R�A�摜�̍X�V����
	UpdateScoreSprite(elapsedTime);

	if (G3_SoratobuHusenWari::score > score->GetMaxOku())
		G3_SoratobuHusenWari::score = score->GetMaxOku();

	// �Ȃɂ��{�^�����������烍�[�f�B���O�V�[��������ŃQ�[���V�[���֐؂�ւ�
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y;
	if ((gamePad.GetButtonDown() & anyButton && !setFade && !fade->GetFade()) && !scoreDirector)
	{
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			0.5f, 0.2f);

		setFade = true;
	}
	else if (setFade && !fade->GetFade())
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

		// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// �`�揈��
void G3_SoratobuHusenWari_Result::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A&�����_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; //RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2D�X�v���C�g�`��
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(backGround->GetTextureWidth());
		float textureHeight = static_cast<float>(backGround->GetTextureHeight());
		
		float color = SceneGameSelect::clear.soratobuHusenWari ? 0.5f : 1.0f;
		
		
		// �^�C�g���X�v���C�g�`��
		backGround->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			color, color, color, 1);

		if (!SceneGameSelect::clear.soratobuHusenWari)
		{
			hint->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}

		textureWidth = static_cast<float>(result->GetTextureWidth());
		textureHeight = static_cast<float>(result->GetTextureHeight());

		// ���U���g
		result->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		// �X�R�A
		score->RenderOku(dc, left, G3_SoratobuHusenWari::score, false,
			scorePos.x, scorePos.y, scoreScale.x, scoreScale.y, 0, space);

		fade->Render(dc, graphics);
	}

#ifndef _DEBUG
	{
		if (ImGui::Begin("Debug"))
		{
			ImGui::DragInt("Score", &G3_SoratobuHusenWari::score, 1000);
			ImGui::Checkbox("Left", &left);
			ImGui::DragFloat2("ScorePos", &scorePos.x, 0.1f);
			ImGui::DragFloat2("ScoreScale", &scoreScale.x, 0.1f);
			ImGui::DragFloat("Space", &space, 0.1f);
		}
		ImGui::End();
	}
#endif // !_DEBUG

}

// �X�R�A�摜�̍X�V����
void G3_SoratobuHusenWari_Result::UpdateScoreSprite(float elapsedTime)
{
	switch (scoreStep)
	{
	case 0:

		scoreTimer += elapsedTime;

		if (scoreTimer > 4.0f)
		{
			scoreTimer = 0.0f;
			scoreStep++;
		}

		break;
	case 1:
	{
		scoreTimer += elapsedTime;

		float screenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth());
		float screenHeight = static_cast<float>(Graphics::Instance().GetScreenHeight());

		float t = scoreTimer / 1.0f;

		if (t < 1.0f)
		{
			scorePos.x = Easing::EaseOut(startScorePosX, 973.5f, t);
		}
		else
		{
			scoreTimer = 0.0f;
			scoreStep++;
		}
	}
		break;
	case 2:

		scoreTimer += elapsedTime;

		if (scoreTimer > 2.0f)
		{
			scoreDirector = false;
		}

		break;
	default:
		break;
	}
}