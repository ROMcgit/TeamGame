#include <memory>
#include <sstream>

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Framework.h"
#include "Audio/BgmManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Scene/SceneManager.h"
#include "Game/Scene/SceneTitle.h"
#include "Game/Scene/SceneGameSelect.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Scene/G1_DarumasangaKoronda.h"
#include "Game/Scene/G2_Sundome.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Game/Scene/G4_OssanTataki.h"
#include "Game/Scene/G5_Asibawatari.h"

//static SceneGameSelect sceneGame;

// ���������Ԋu�ݒ�
static const int syncInterval = 1;

// �R���X�g���N�^
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
	, input(hWnd)
	, graphics(hWnd)
{
	// �G�t�F�N�g�}�l�[�W���[������
	EffectManager::Instance().Initialize();

	// �V�[��������
	//sceneGame.Initialize();
	SceneManager::Instance().ChangeScene(std::make_unique<SceneTitle>());
}

// �f�X�g���N�^
Framework::~Framework()
{
	// �V�[���I����
	//sceneGame.Finalize();
	SceneManager::Instance().Clear();

	// �G�t�F�N�g�}�l�[�W���[�I����
	EffectManager::Instance().Finalize();
}

// �X�V����
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// ���͍X�V����
	input.Update();

	// �V�[���X�V����
	//sceneGame.Update(elapsedTime);
	SceneManager::Instance().Update(elapsedTime);
}

// �`�揈��
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// �ʃX���b�h���Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
	// �����A�N�Z�X���Ȃ��悤�ɔr�����䂷��
	std::lock_guard<std::mutex> lock(graphics.GetMutex());

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// IMGUI�t���[���J�n����
	graphics.GetImGuiRenderer()->NewFrame();

	// �V�[���`�揈��
	//sceneGame.Render();
	SceneManager::Instance().Render();

	// IMGUI�f���E�C���h�E�`��iIMGUI�@�\�e�X�g�p�j
	//ImGui::ShowDemoWindow();

	// �V�[���؂�ւ�GUI
	SceneSelectGUI();

	// IMGUI�`��
	graphics.GetImGuiRenderer()->Render(dc);

	// �o�b�N�o�b�t�@�ɕ`�悵�������ʂɕ\������B
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

template<class T>
void Framework::ChangeSceneButtonGUI(const char* name)
{
	//TODO:�N���b�N�ŃV�[����؂�ւ���
	if (ImGui::Button(name))
	{
		BgmManager::Instance().StopBgm();
		SceneManager::Instance().ChangeScene(std::make_unique<T>());
	}
}

// �V�[���؂�ւ�GUI
void Framework::SceneSelectGUI()
{
#ifndef _DEBUG

	//! �ʒu�ݒ�
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	//! �傫���ݒ�
	ImGui::SetNextWindowSize(ImVec2(320, 720), ImGuiCond_Always);

	/*! ImGui�ݒ� */
#if 1
	//! �s�����x
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.5f, 0.0f, 0.0f, 0.8f));

	/*! ��؂�� */
	ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));         // �ʏ�̐F
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(1.0f, 0.7f, 0.3f, 1.0f));  // �z�o�[���̐F
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(1.0f, 1.0f, 0.5f, 1.0f));   // �N���b�N���̐F

	/*! �w�b�_�[ */
	// �w�i�F
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
	// �z�o�[���̐F
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));
	// �N���b�N���̐F
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.5f, 0.8f, 1.0f, 1.0f));

	/*! �{�^�� */
	// �w�i�F
	ImGui::PushStyleColor(ImGuiCol_Button,
		ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
	// �z�o�[���̐F
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
		ImVec4(0.3f, 0.6f, 0.8f, 1.0f));
	// �N���b�N���̐F
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
		ImVec4(0.9f, 0.7f, 0.2f, 1.0f));

	/*! �t���[�� */
	// �w�i�F
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
	// �z�o�[���̐F
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
		ImVec4(0.3f, 0.6f, 0.8f, 1.0f));
	// �N���b�N���̐F
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive,
		ImVec4(0.5f, 0.8f, 1.0f, 1.0f));

	/*! �`�F�b�N�}�[�N�̐F */
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	//! ���̐F
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
	//! ���̑�����2.0�ɐݒ�
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);


	//! �{�^���̊p�ۂ̓x����
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	//! �{�^���̓����̗]��
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));

#endif

	if (ImGui::Begin("GameDebug"))
	{
		/*! �J�����Ȃǂ̏��� */
#if 1
		//! �J�����R���g���[���[
		if (ImGui::CollapsingHeader("CameraController"))
#if 1
		{
			// �f�o�b�O�J����
			ImGui::Checkbox(u8"�f�o�b�O�J����", &CameraController::debugCamera);
			ImGui::Checkbox(u8"�f�o�b�O�J�����̈ړ���~", &CameraController::noMoveDebugCamera);

			// �^�[�Q�b�g
			ImGui::DragFloat3(u8"�^�[�Q�b�g", &CameraController::target.x, 0.1f);

			// ��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(CameraController::angle.x);
			a.y = DirectX::XMConvertToDegrees(CameraController::angle.y);
			a.z = DirectX::XMConvertToDegrees(CameraController::angle.z);
			ImGui::DragFloat3(u8"�p�x", &a.x, 0.1f);
			CameraController::angle.x = DirectX::XMConvertToRadians(a.x);
			CameraController::angle.y = DirectX::XMConvertToRadians(a.y);
			CameraController::angle.z = DirectX::XMConvertToRadians(a.z);

			// �J�����͈̔�
			ImGui::DragFloat(u8"�͈�", &CameraController::range, 0.1f);
		}

#endif
		//! ���C�g
		if (ImGui::TreeNode(u8"���C�g�Ȃ�"))
#if 1
		{
			ImGui::Checkbox(u8"���l���f�o�b�O�ŕύX����", &Scene::debugControll);
			if (ImGui::CollapsingHeader(u8"���C�g���"))
			{
				ImGui::DragFloat4(u8"����", &Scene::debugLightDirection.x, 0.01f);
				ImGui::ColorEdit4(u8"�F(���C�g)", &Scene::debugLightColor.x);
				ImGui::InputFloat3(u8"�F�̐��l", &Scene::debugLightColor.x);
				ImGui::DragFloat3(u8"�ʒu", &Scene::debugLightPosition.x, 0.1f);
				ImGui::DragFloat(u8"�͈�", &Scene::debugLightRange, 0.1f);
			}
			if (ImGui::CollapsingHeader(u8"�V���h�E�}�b�v"))
			{
				ImGui::DragFloat3(u8"�ʒu�̃I�t�Z�b�g", &Scene::debugShadowMapEyeOffset.x, 0.1f);
				ImGui::DragFloat(u8"�o�C�A�X", &Scene::debugShadowMapBias, 0.001f);
				ImGui::DragFloat(u8"���͈̔�", &Scene::debugShadowMapWidth, 0.01f);
				ImGui::DragFloat(u8"�c�͈̔�", &Scene::debugShadowMapLength, 0.01f);
				ImGui::DragFloat(u8"�J�n(�e)", &Scene::debugShadowMapStart, 0.01f);
				ImGui::DragFloat(u8"�I��(�e)", &Scene::debugShadowMapEnd, 0.01f);
			}
			if (ImGui::CollapsingHeader(u8"���̉e��"))
			{
				ImGui::DragFloat(u8"����", &Scene::debugAmbientStrength, 0.01f);
				ImGui::DragFloat(u8"�g�U��", &Scene::debugDiffuseStrength, 0.01f);
				ImGui::DragFloat(u8"�X�y�L�����[��", &Scene::debugSpecularStrength, 0.01f);
			}
			//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-----------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"�t�H�O"))
			{
				ImGui::ColorEdit3(u8"�F(�t�H�O)", &Scene::debugFogColor.x);
				ImGui::DragFloat(u8"�J�n(�t�H�O)", &Scene::debugFogStart);
				ImGui::DragFloat(u8"�I��(�t�H�O)", &Scene::debugFogEnd);
			}

			ImGui::TreePop();
		}
#endif
		//! �|�X�g�G�t�F�N�g
		if (ImGui::TreeNode(u8"�|�X�g�G�t�F�N�g"))
#if 1
		{
			//! ���l�����Z�b�g����
			ImGui::Checkbox(u8"���l�����Z�b�g����", &Camera::postEffectReset);

			if (ImGui::CollapsingHeader(u8"�F�֘A"))
			{
				// �R���g���X�g(���l���傫���قǁA���邢�����͂�薾�邭�A�Â������͂��Â��Ȃ�)
				ImGui::DragFloat(u8"�R���g���X�g", &Camera::postEffect.contrast, 0.01f);
				// �T�`�����[�V����(�F�̍ʓx)
				ImGui::DragFloat(u8"�ʓx", &Camera::postEffect.saturation, 0.01f);
				// �J���[�t�B���^�[(�F�t�B���^�[)
				ImGui::DragFloat3(u8"�F�t�B���^�[", &Camera::postEffect.colorFilter.x, 0.01f);
				// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))
				ImGui::DragFloat(u8"�F�Y��", &Camera::postEffect.chromaticAberration, 0.01f);
				// �F���V�t�g
				ImGui::DragFloat(u8"�F���V�t�g", &Camera::postEffect.hueShift, 0.01f, 0.0f, 1.0f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"�r�l�b�g"))
			{
				// �r�l�b�g�̋��x
				ImGui::DragFloat(u8"�r�l�b�g�̋��x", &Camera::postEffect.vignetteIntensity, 0.01f, 0.0f, 1.0f);
				// �r�l�b�g�̕s�����x
				ImGui::DragFloat(u8"�r�l�b�g�̕s�����x", &Camera::postEffect.vignetteOpacity, 0.01f, 0.0f, 1.0f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"�u���[�֘A"))
			{
				// �u���[���x
				ImGui::DragFloat(u8"�u���[���x", &Camera::postEffect.blurStrength, 0.01f);
				// �s���g�����킹�鋗��
				ImGui::DragFloat(u8"�s���g�����킹�鋗��", &Camera::postEffect.focusDistance, 0.1f);
				// �s���g�������͈�
				ImGui::DragFloat(u8"�s���g�������͈�", &Camera::postEffect.focusRange, 0.1f);
				// ��ʊE�[�x�̃u���[���x
				ImGui::DragFloat(u8"��ʊE�[�x�̃u���[���x", &Camera::postEffect.dofBlurStrength, 0.1f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"�O���A"))
			{
				// �O���A�̋��x
				ImGui::DragFloat(u8"�O���A��臒l", &Camera::postEffect.bloomThreshold, 0.01f);
				// �O���A�̋��x
				ImGui::DragFloat(u8"�O���A�̋��x", &Camera::postEffect.bloomIntensity, 0.01f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"�J�����V�F�C�N"))
			{
				// �V�F�C�N�̃I�t�Z�b�g
				ImGui::DragFloat2(u8"�V�F�C�N�̃I�t�Z�b�g", &Camera::postEffect.shakeOffset.x, 0.01f);
				// �V�F�C�N�̑���
				ImGui::DragFloat(u8"�V�F�C�N�̑���", &Camera::postEffect.shakeSpeed, 0.1f);
				// �V�F�C�N�̋��x
				ImGui::DragFloat(u8"�V�F�C�N�̋��x", &Camera::postEffect.shakeStrength, 0.01f);
				// �V�F�C�N�̎���
				ImGui::DragFloat(u8"�V�F�C�N�̎���", &Camera::postEffect.shakeTime, 0.01f);
			}

			ImGui::TreePop();
		}
#endif

#endif
	}
	// �I��
	ImGui::End();

	//! �ʒu�ݒ�
	ImGui::SetNextWindowPos(ImVec2(1050, 0), ImGuiCond_Always);
	//! �傫���ݒ�
	ImGui::SetNextWindowSize(ImVec2(230, 500), ImGuiCond_Always);

	if (ImGui::Begin("Scene"))
	{
		ChangeSceneButtonGUI<SceneTitle>(u8"00.�^�C�g�����");
		ChangeSceneButtonGUI<SceneGameSelect>(u8"01.�Q�[�����");

		if (ImGui::TreeNode(u8"�Q�[���V�[��"))
		{
			ChangeSceneButtonGUI<G0_Onigokko>          (u8"00.���ɂ�����");
			ChangeSceneButtonGUI<G1_DarumasangaKoronda>(u8"01.����܂��񂪓]��");
			ChangeSceneButtonGUI<G2_Sundome>           (u8"02.���~��");
			ChangeSceneButtonGUI<G3_SoratobuHusenWari> (u8"03.���ԕ��D����");
			ChangeSceneButtonGUI<G4_OssanTataki>       (u8"04.���ɂ�����");
			ChangeSceneButtonGUI<G5_Asibawatari>       (u8"05.���ɂ�����");

			ImGui::TreePop();
		}
	}
	ImGui::PopStyleVar(3);    // PushStyleVar�Ŏw�肵��������Pop����
	ImGui::PopStyleColor(15); // PushStyleColor�Ŏw�肵��������Pop����

	ImGui::End();

#endif // !_DEBUG
}

// �t���[�����[�g�v�Z
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << "��㕽�색���h " "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// �A�v���P�[�V�������[�v
int Framework::Run()
{
	MSG msg = {};

	HDC hDC = GetDC(hWnd);

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
			CalculateFrameStats();

			// TODO: elapsedTime(�L�����N�^�[�̈ړ����x�ɉe��)
			float elapsedTime = syncInterval == 0
				? timer.TimeInterval()
				: syncInterval / static_cast<float>(GetDeviceCaps(hDC, VREFRESH))
				;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}
	
	ReleaseDC(hWnd, hDC);

	return static_cast<int>(msg.wParam);
}

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Graphics::Instance().GetImGuiRenderer()->HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
