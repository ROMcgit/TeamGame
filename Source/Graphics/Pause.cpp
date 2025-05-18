#include "Pause.h"
#include "Graphics.h"
#include "Fade.h"

#include "Input/Input.h"
#include "Game/Scene/SceneGameSelect.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneManager.h"
#include "Game/Camera/CameraController.h"

#include "Audio/BgmManager.h"
#include "Audio/SoundManager.h"

// �R���X�g���N�^
Pause::Pause()
{
	//! ���ʉ�
	SoundManager& sound = SoundManager::Instance();
	sound.LoadSound("�\��", "Data/Audio/Sound/Decision.wav");
	sound.LoadSound("�^�C�g���֖߂�", "Data/Audio/Sound/Cancel.wav");

	pauseBG = std::make_unique<Sprite>("Data/Sprite/Pause/Pause.png");

	//! �|�[�Y��ʑI��
	for (int i = 0; i < 2; i++)
	{
		std::string filePath = "Data/Sprite/Pause/PauseSelect" + std::to_string(i) + ".png";

		pauseSelectImage[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	pauseExplanation = std::make_unique<Sprite>("Data/Sprite/Pause/PauseExplanation.png");

	//! �t�F�[�h
	fade = std::make_unique<Fade>();
}

Pause::~Pause()
{
}

// �X�V����
void Pause::Update(float elapsedTime)
{
	//! �f�o�b�O�J��������Ȃ��Ȃ�
	if (!CameraController::debugCamera)
	{
		//! �t�F�[�h�̍X�V����
		fade->Update(elapsedTime);

		//! �|�[�Y��ʂ̏���
		UpdatePause(elapsedTime);
	}
}

// �`�揈��
void Pause::Render(ID3D11DeviceContext* dc, Graphics& graphics)
{
	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	float textureWidth = static_cast<float>(pauseExplanation->GetTextureWidth());
	float textureHeight = static_cast<float>(pauseExplanation->GetTextureHeight());

	float opacity = 1.0f - pauseOpacity;

	//! �|�[�Y��ʂ̐���
	pauseExplanation->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		0.8f, 0.8f, 0.8f, opacity);

	textureWidth = static_cast<float>(pauseBG->GetTextureWidth());
	textureHeight = static_cast<float>(pauseBG->GetTextureHeight());

	//! �|�[�Y��ʂ̔w�i
	pauseBG->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		0.5f, 0.5f, 0.5f, pauseOpacity);

/************************************************************************************************************/

	//! �|�[�Y��ʂ̃{�[�h
	for (int i = 0; i < 2; i++)
	{
		textureWidth = static_cast<float>(pauseSelectImage[i]->GetTextureWidth());
		textureHeight = static_cast<float>(pauseSelectImage[i]->GetTextureHeight());

		if(i + 1 == pauseSelect)
		{
			pauseSelectImage[i]->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, pauseOpacity);
		}
		else
		{
			pauseSelectImage[i]->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				0.5f, 0.5f, 0.5f, pauseOpacity);
		}
	}

	//! ����ȕ`�揈��
	{
		//! �t�F�[�h�̕`�揈��
		fade->Render(dc, graphics);
	}
}

// �|�[�Y��ʂ̏���
void Pause::UpdatePause(float elapsedTime)
{
	//! �{�^�����̃C���X�^���X�擾
	GamePad& gamePad = Input::Instance().GetGamePad();

	//! ���ʉ�
	SoundManager& sound = SoundManager::Instance();

	//! �|�[�Y��ʂ��ᖳ���Ȃ�
	if (!pause)
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_START && pauseOpacity <= 0.0f)
		{
			//! �|�[�Y��ʂɂ���
			sound.PlaySound("�\��");
			pause = true;
		}

		//! �s�����x��0.0f���傫���Ȃ�A���l��������
		if (pauseOpacity > 0.0f)
			pauseOpacity -= pauseOpacitySpeed * elapsedTime;
		else
		{
			pauseSelect = 1;
		}
	}
	//! �|�[�Y��ʒ��Ȃ�
	else
	{
		//! ��ʂ��\���d�؂�����A������󂯂���
		if (pauseOpacity >= 1.0f)
		{
			//! �{�^���̓��͏��
			int buttonState = gamePad.GetButtonDown();

			if (!setFade)
			{
				switch (buttonState)
				{
				case GamePad::BTN_START:
				case GamePad::BTN_B:
				{
					//! �|�[�Y��ʂ��I��
					sound.PlaySound("�\��");
					pause = false;
				}
				break;
				case GamePad::BTN_UP:
				{
					if (pauseSelect == 2)
					{
						pauseSelect--;
					}

				}
				break;
				case GamePad::BTN_DOWN:
				{
					if (pauseSelect == 1)
					{
						pauseSelect++;
					}
				}
				break;
				case GamePad::BTN_A:
				{
					//! �I���ɂ���ď�����ς���
					switch (pauseSelect)
					{
					case 1:
					{
						//! �|�[�Y��ʂ��I��
						sound.PlaySound("�\��");
						pause = false;
					}
					break;
					case 2:
					{
						//! ���ʉ�
						sound.PlaySound("�^�C�g���֖߂�");

						//! �t�F�[�h��ݒ肵����
						fade->SetFade(DirectX::XMFLOAT3(1, 1, 1),
							0.0f, 1.0f,
							0.5f, 0.2f);

						setFade = true;
					}
					break;
					default:
						break;
					}
				}
				break;
				default:
					break;
				}
			}
			//! �t�F�[�h��ݒ肵���Ȃ�
			else if (setFade && !fade->GetFade())
			{
				//! �V�[���؂�ւ�����
				SceneChange();
			}
		}

		//! �s�����x��1.0f�����Ȃ�A���l���グ��
		if (pauseOpacity < 1.0f)
			pauseOpacity += pauseOpacitySpeed * elapsedTime;
	}
}

// �V�[���؂�ւ�����
void Pause::SceneChange()
{
	BgmManager& bgm = BgmManager::Instance();

	bgm.UnloadBgm("���ɂ�����");
	bgm.UnloadBgm("����܂��񂪓]��");
	bgm.UnloadBgm("���~��");
	bgm.UnloadBgm("���ԕ��D����");
	bgm.UnloadBgm("��������@��");
	bgm.UnloadBgm("����n��");

	std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

	// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
	SceneManager::Instance().ChangeScene(std::move(loadingScene));
}