#include "Graphics/Graphics.h"
#include "Game/Scene/G2_Sundome_Result.h"
#include "Game/Scene/G2_Sundome.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Fade.h"
#include "Other/Easing.h"
#include "Audio/BgmManager.h"

// ������
void G2_Sundome_Result::Initialize()
{
	// ���U���g
	result = std::make_unique<Sprite>("Data/Sprite/Result.png");

	// �q���g
	hint = std::make_unique<Sprite>("Data/Sprite/2.Sundome/Hint.png");

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);

	for(int i = 0; i < 4; i++)
	{
		text[i] = std::make_unique<Text>();
	}

	float screeenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth());
	for (int i = 0; i < 4; i++)
	{
		textPos[i].x = screeenWidth * 1.1f;
	}

	 startTextPosX = textPos[0].x;

	float screenHeight = static_cast<float>(Graphics::Instance().GetScreenHeight());
	textPos[0].y = screenHeight * 0.25f;
	textPos[1].y = screenHeight * 0.45f;
	textPos[2].y = screenHeight * 0.65f;
	textPos[3].y = screenHeight * 0.85f;

	//! �X�R�A����
	int score[3];
	for (int i = 0; i < 3; i++)
		score[i] = G2_Sundome::score[i];

	//! ���v�X�R�A���v�Z
	int totalScore = score[0] + score[1] + score[2];
	this->totalScore = totalScore;

	if (this->totalScore >= 80)
		SceneGameSelect::clear.sundome = true;

	std::string filePath = "";
	if (SceneGameSelect::clear.sundome)
		filePath = "Data/Sprite/2.Sundome/Bonus.png";
	else
		filePath = "Data/Sprite/GameSelect/2.png";

	// �w�i
	backGround = std::make_unique<Sprite>(filePath.c_str());

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("���U���g", "Data/Audio/Bgm/6.Result.wav");
	bgm.PlayBgm("���U���g", 0.8f);
}

// �I����
void G2_Sundome_Result::Finalize()
{
}

// �X�V����
void G2_Sundome_Result::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	fade->Update(elapsedTime);

	// �X�R�A���o����
	DirectorScore(elapsedTime);

	// �Ȃɂ��{�^�����������烍�[�f�B���O�V�[��������ŃQ�[���V�[���֐؂�ւ�
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y;
	if ((gamePad.GetButtonDown() & anyButton && !setFade && !fade->GetFade()) && directorFinish)
	{
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			0.5f, 0.2f);

		setFade = true;
	}
	else if (setFade && !fade->GetFade())
	{
		BgmManager::Instance().UnloadBgm("���U���g");

		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

		// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// �`�揈��
void G2_Sundome_Result::Render()
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
		// �w�i
		backGround->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			0.5f, 0.5f, 0.5f, 1);

		if (!SceneGameSelect::clear.sundome)
		{
			hint->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}

		textureWidth = static_cast<float>(result->GetTextureWidth());
		textureHeight = static_cast<float>(result->GetTextureHeight());

		// ���U���g����
		result->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		// �X�R�A
		for (int i = 0; i < 3; i++)
		{
			text[i]->RenderThousand(dc, false,
				G2_Sundome::score[i],
				false, false, false,
				textPos[i].x, textPos[i].y);
		}

		//! ���v�X�R�A
		text[3]->RenderThousand(dc, false,
			totalScore,
			false, false, false,
			textPos[3].x, textPos[3].y,
			12.0f, 12.0f, 0,
			40.0f);

		fade->Render(dc, graphics);
	}

#ifndef _DEBUG

	{
		if (ImGui::Begin("Debug"))
		{
			ImGui::InputInt("Step", &directorStep);
			for(int i = 0; i < 4; i++)
			{
				std::string name = "TextPos" + std::to_string(i);
				ImGui::DragFloat2(name.c_str(), &textPos[i].x, 0.5f);
			}

			for (int i = 0; i < 3; i++)
			{
				std::string name = "Score" + std::to_string(i);
				ImGui::InputInt(name.c_str(), &G2_Sundome::score[i], 2);
			}
		}
		ImGui::End();
	}

#endif // !_DEBUG
}

// �X�R�A�̉��o����
void G2_Sundome_Result::DirectorScore(float elapsedTime)
{
	if (directorFinish) return;

	float screeenWidth = Graphics::Instance().GetScreenWidth();
	float end = screeenWidth * 0.8f;

	switch (directorStep)
	{
	case 0:

		directorTime += elapsedTime;

		if (directorTime > 2.0f)
		{
			directorTime = 0.0f;
			directorStep++;
		}

		break;
	//! textPos[textNum].x�̕ύX
	case 1:
	{
		

		directorTime += elapsedTime;

		float t = directorTime / 0.8f;

		if (t < 1.0f)
		{
			textPos[textNum].x = Easing::EaseOut(startTextPosX, end, t);
		}
		else
		{
			directorTime = 0.0f;
			directorStep++;
			textNum++;
		}
	}
		break;
	//! �҂�����
	case 2:

		directorTime += elapsedTime;

		if (textNum >= 5)
		{
			directorTime = 0.0f;
			directorStep++;
		}
		else
		{
			float waitTime = textNum == 3 ? 0.6f : 0.3f;
			if (directorTime > waitTime)
			{
				directorTime = 0.0f;
				directorStep = 1;
			}
		}
		break;
	case 3:

		directorTime += elapsedTime;

		if (directorTime > 1.0f)
		{
			directorFinish = true;
		}

		break;
	default:
		break;
	}
}