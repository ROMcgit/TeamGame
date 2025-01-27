#include "CameraController.h"
#include "Camera.h"
#include "Graphics/Fade.h"
#include "Game/Character/Director/DirectorManager.h"
#include "Game/Character/Director/CameraTarget.h"
#include "Input/Input.h"
#include "Audio/SoundManager.h"
#include "Other/Easing.h"
#include <imgui.h>

/*! �ǂ̃C�[�W���O�ɂ��邩 */

//! �^�[�Q�b�g
CameraController::TargetChangeEasing CameraController::targetChangeEasing = CameraController::TargetChangeEasing::Linear;
//! �p�x
CameraController::AngleChangeEasing CameraController::angleChangeEasing = CameraController::AngleChangeEasing::Linear;
//! �͈�
CameraController::RangeChangeEasing CameraController::rangeChangeEasing = CameraController::RangeChangeEasing::Linear;

//! �^�[�Q�b�g
DirectX::XMFLOAT3 CameraController::target = { 0, 0, 0 };

//! �^�[�Q�b�g�̈ʒu��ϕς��邩
bool CameraController::targetChange = false;
//! �^�[�Q�b�g�̕ύX�̊J�n�̒l
DirectX::XMFLOAT3 CameraController::startTargetChange = { 0, 0, 0 };
//! �����܂Ń^�[�Q�b�g�̈ʒu��ς���
DirectX::XMFLOAT3 CameraController::endTargetChange = { 0, 0, 0 };
//! �^�[�Q�b�g�̈ʒu��ς��鎞��
float CameraController::targetChangeTime = 0.0f;
//! �^�[�Q�b�g�̈ʒu�ύX�̌o�ߎ���
float CameraController::targetChangeCurrentTime = 0.0f;

/********************************************************************/

//! �p�x
DirectX::XMFLOAT3 CameraController::angle = { DirectX::XMConvertToRadians(26),0,0 };

//! �p�x��ς��邩
bool CameraController::angleChange = false;
//! �p�x�̕ύX�̊J�n�̒l
DirectX::XMFLOAT3 CameraController::startAngleChange = { 0, 0, 0 };
//! �����܂Ŋp�x��ς���
DirectX::XMFLOAT3 CameraController::endAngleChange = { 0, 0, 0 };
//! �p�x��ς��鎞��
float CameraController::angleChangeTime = 0.0f;
//! �p�x�ύX�̌o�ߎ���
float CameraController::angleChangeCurrentTime = 0.0f;

/********************************************************************/

//! �J�����͈̔�
float CameraController::range = 16.0f;

//! �J�����͈̔͂�ς��邩
bool CameraController::rangeChange = false;
//! �J�����͈̔͂̕ύX�̊J�n�̒l
float CameraController::startRangeChange = 0.0f;
//! �����܂ŃJ�����͈̔͂�ς���
float CameraController::endRangeChange = 0.0f;
//! �J�����͈̔͂�ς��鎞��
float CameraController::rangeChangeTime = 0.0f;
//! �J�����͈͕̔ύX�̌o�ߎ���
float CameraController::rangeChangeCurrentTime = 0.0f;

/********************************************************************/

//! �J������h�炷��
bool CameraController::cameraShake = false;
//! �h�炷����
float CameraController::cameraShakeTimer = 0.0f;
//! �h�炷�傫��
DirectX::XMINT3 CameraController::cameraShakePower = { 0, 0, 0 };

/********************************************************************/

//! �f�o�b�O�J����
bool CameraController::debugCamera = false;
//! �f�o�b�O�J�������Ɉړ����Ȃ��悤�ɂ��邩
bool CameraController::noMoveDebugCamera = false;

// �R���X�g���N�^
CameraController::CameraController()
{
	//! �ϐ���������
	targetChange = false; // �^�[�Q�b�g�̈ʒu��ϕς��邩
	angleChange = false; // �p�x��ς��邩
	rangeChange = false; // �J�����͈̔͂�ς��邩

	cameraShake = false;       // �J������h�炷��
	cameraShakeTimer = 0.0f;        // �J������h�炷����
	cameraShakePower = { 0, 0, 0 }; // �h�炷�傫��

	debugCamera = false; // �f�o�b�O�J����
	noMoveDebugCamera = false; // �f�o�b�O�J�������Ɉړ����Ȃ��悤�ɂ��邩


	// �t�F�[�h�𐶐�
	fade = std::make_unique<Fade>();

	SoundManager::Instance().LoadSound("�{�X���j", "Data/Audio/Sound/Enemy/Defeat.wav");

#ifndef _DEBUG
	//! �J�����̃^�[�Q�b�g�𐶐�
	std::unique_ptr<CameraTarget> cameraTarget = std::make_unique<CameraTarget>();
	DirectorManager::Instance().Register(std::move(cameraTarget));
#endif
}

// �f�X�g���N�^
CameraController::~CameraController()
{
}

// �X�V����
void CameraController::Update(float elapsedTime)
{
	// �t�F�[�h�X�V����
	fade->Update(elapsedTime);

	// ���o�}�l�[�W���[�̍X�V����
	DirectorManager::Instance().Update(elapsedTime);

#if 1
	// �J�����̉�]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// ��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	// �J�����̏�ԍX�V����
	UpdateCameraState(elapsedTime);

	// �J�����̎��_�ƒ����_��ݒ�
	Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
#endif
	// �J�����̃^�[�Q�b�g�X�V����
	UpdateCameraTargetTracking(elapsedTime);
}

// �`�揈��
void CameraController::RenderCameraTarget(ID3D11DeviceContext* dc, Shader* shader)
{
	//! �f�o�b�O�J�������ɕ\��
	DirectorManager::Instance().Render(dc, shader);
}

// �t�F�[�h��`��
void CameraController::FadeRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	// �t�F�[�h��`��
	fade->Render(dc, graphics);
}

// �J�����̃f�o�b�OGUI
void CameraController::DrawDebugGUI()
{
	if (ImGui::TreeNode("CameraController"))
	{
		// �f�o�b�O�J������
		ImGui::Checkbox("DebugCamera", &debugCamera);
		// �f�o�b�O�J�����̓������~�߂邩
		ImGui::Checkbox("NoMoveDebugCamera", &noMoveDebugCamera);
		// ���ڂ��邩
		ImGui::Checkbox("Tracking", &tracking);

		if (ImGui::CollapsingHeader("Transform"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			// �^�[�Q�b�g
			ImGui::DragFloat3("Target", &target.x, 0.1f);

			// ��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::DragFloat3("Angle", &a.x, 0.1f);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);

			// �J�����͈̔�
			ImGui::DragFloat("Range", &range, 0.1f);
		}

		if (ImGui::CollapsingHeader("Status"))
		{
			// ����
			ImGui::DragFloat("height", &height, 0.1f);
			// ����
			ImGui::InputFloat("Dist", &dist);
		}

		ImGui::TreePop();
	}
}

// �J�����̒��ڍX�V����
void CameraController::UpdateCameraTargetTracking(float elapsedTime)
{
}

// �J�����̏�ԍX�V����
void CameraController::UpdateCameraState(float elapsedTime)
{
	// �J�����̐�������
	CameraLimit();

	// �f�o�b�O�J����
	UpdateDebugCamera(elapsedTime);

	// �J�����̈ʒu�̕ύX�X�V����
	UpdateTargetChange(elapsedTime);

	// �J�����̊p�x�̕ύX�X�V����
	UpdateAngleChange(elapsedTime);

	// �J�����͈̔͂̕ύX�X�V����
	UpdateRangeChange(elapsedTime);

	// �J�����V�F�C�N�X�V����
	UpdateCameraShake(elapsedTime);

	// �J�������[�r�[���ԏ���
	UpdateCameraMovieTimer(elapsedTime);

	// ���̍X�V�������~�߂�X�V����
	UpdateBossFinish(elapsedTime);
}

// �J�����̐�������
void CameraController::CameraLimit()
{
	if (range < minRange) range = minRange;      // �J�����͈͍ŏ�
	else if (range > maxRange) range = maxRange; // �J�����͈͍ő�

	/// X���̃J������]�𐧌�
	if (angle.x < minAngleX) angle.x = minAngleX;
	if (angle.x > maxAngleX) angle.x = maxAngleX;

	// Y���̉�]�l��-3.14�`3.14�Ɏ��߂�
	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI)  angle.y -= DirectX::XM_2PI;
}

// �f�o�b�O�J����
bool CameraController::UpdateDebugCamera(float elapsedTime)
{
#ifndef _DEBUG

	Mouse& mouse = Input::Instance().GetMouse();
	//! ImGui�𑀍쒆�͏��������Ȃ�
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
	{
		debugCamera = false; // �f�o�b�O�J����������
		noMoveDebugCamera = false; // �f�o�b�O�J�����̓������~�߂�̂���������

		return false;
	}
	//! �Q�[����ʂ��N���b�N������A�f�o�b�O�J�����ɐ؂�ւ���
	else
	{
		debugCamera = true;  // �f�o�b�O�J�����ɂ���

		if (tracking)
			tracking = false; // �J�����̒��ڂ�����
	}

	//! �f�o�b�O�J�����̓������~�߂邩
	if (mouse.GetButtonDown() & Mouse::BTN_MIDDLE)
		noMoveDebugCamera = !noMoveDebugCamera;

	//! �f�o�b�O�J�����𓮂����Ȃ��Ȃ�A�����ŏ������~�߂�
	if (noMoveDebugCamera) return false;

	//! �{�^���Ń^�[�Q�b�g�̈ړ�
	{
		Camera& camera = Camera::Instance();

		// �J�����̑O�����A�E�����A��������擾
		DirectX::XMFLOAT3 front = camera.GetFront();
		DirectX::XMFLOAT3 right = camera.GetRight();
		DirectX::XMFLOAT3 up = camera.GetUp();

		// �ړ������̃x�N�g����������
		DirectX::XMFLOAT3 moveDirection = { 0.0f, 0.0f, 0.0f };

		// �L�[���͂ňړ�����������
		GamePad& gamePad = Input::Instance().GetGamePad();

		//! Shift�L�[�������Ă���Ȃ�
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			// �ړ����x            // �㏸���x
			targetMoveSpeed = targetUpSpeed = 15.0f;
		}
		else
		{
			// �ړ����x            // �㏸���x
			targetMoveSpeed = targetUpSpeed = 8.0f;
		}

		//! �㏸
		if (gamePad.GetButtonHeld() & GamePad::BTN_UP && GetAsyncKeyState(VK_CONTROL) & 0x8000 && !(gamePad.GetButtonHeld() & GamePad::BTN_DOWN))
		{
			// ���̂܂�Y���W���ړ�������
			target.y += targetUpSpeed * elapsedTime;
		}
		//! �O�i
		else if (gamePad.GetButtonHeld() & GamePad::BTN_UP && !(gamePad.GetButtonHeld() & GamePad::BTN_DOWN))
		{
			moveDirection.x += front.x;
			moveDirection.y += front.y;
			moveDirection.z += front.z;
		}

		//! ���~
		if (gamePad.GetButtonHeld() & GamePad::BTN_DOWN && GetAsyncKeyState(VK_CONTROL) & 0x8000 && !(gamePad.GetButtonHeld() & GamePad::BTN_UP))
		{
			// ���̂܂�Y���W���ړ�������
			target.y -= targetUpSpeed * elapsedTime;
		}
		// ���
		else if (gamePad.GetButtonHeld() & GamePad::BTN_DOWN && !(gamePad.GetButtonHeld() & GamePad::BTN_UP))
		{
			moveDirection.x -= front.x;
			moveDirection.z -= front.z;
		}

		// ���Ɉړ�
		if (gamePad.GetButtonHeld() & GamePad::BTN_LEFT && !(gamePad.GetButtonHeld() & GamePad::BTN_RIGHT))
		{
			moveDirection.x -= right.x;
			moveDirection.z -= right.z;
		}
		// �E�Ɉړ�
		else if (gamePad.GetButtonHeld() & GamePad::BTN_RIGHT && !(gamePad.GetButtonHeld() & GamePad::BTN_LEFT))
		{
			moveDirection.x += right.x;
			moveDirection.z += right.z;
		}

		// �ړ��������[���łȂ��ꍇ�ɏ���(���������ŁA�΂߈ړ��ɂȂ�̂�h�����߁AUP�͏��O)
		//! �����̓��
		float lengthSquared = moveDirection.x * moveDirection.x +
			moveDirection.z * moveDirection.z;

		if (lengthSquared > 0.0f)
		{
			// ���K�����đ��x��K�p
			float length = std::sqrt(lengthSquared);
			moveDirection.x /= length;
			moveDirection.z /= length;

			target.x += moveDirection.x * targetMoveSpeed * elapsedTime;
			target.z += moveDirection.z * targetMoveSpeed * elapsedTime;
		}
	}

	//! �}�E�X���N���b�N������
#if 1
	if (mouse.GetButtonHeld() & Mouse::BTN_LEFT)
	{
		// �}�E�X�̈ړ��ʂɍ��킹�ăJ��������]
		float deltaX = static_cast<float>(mouse.GetPositionX() - mouse.GetOldPositionX());
		float deltaY = static_cast<float>(mouse.GetPositionY() - mouse.GetOldPositionY());

		Camera& camera = Camera::Instance();
		// �J�����̑O�����ƉE�����ƒ����_���擾
		DirectX::XMFLOAT3 up = camera.GetUp();
		DirectX::XMFLOAT3 right = camera.GetRight();
		DirectX::XMFLOAT3 focus = camera.GetFocus();

		float speed = (mouseSensitivity * (range * 0.7f)) * elapsedTime;
		float x = deltaX * speed;
		float y = deltaY * speed;

		// �E����
		focus.x -= right.x * x;
		focus.y -= right.y * x;
		focus.z -= right.z * x;

		// �����
		focus.x += up.x * y;
		focus.y += up.y * y;
		focus.z += up.z * y;

		//! �^�[�Q�b�g�ʒu��ݒ�
		target.x = focus.x;
		target.y = focus.y;
		target.z = focus.z;
	}
#endif

	// IMGUI�̃}�E�X���͒l���g���ăJ�������삷��
	ImGuiIO io = ImGui::GetIO();
	//! �}�E�X�z�C�[��������
	if (io.MouseWheel != 0)
	{
		range -= io.MouseWheel * range * mouseZoomSpeed;
	}

	//! �}�E�X�E�N���b�N������
	if (mouse.GetButtonHeld() & Mouse::BTN_RIGHT)
	{
		// �}�E�X�̈ړ��ʂɍ��킹�ăJ��������]
		int deltaX = mouse.GetPositionX() - mouse.GetOldPositionX();
		int deltaY = mouse.GetPositionY() - mouse.GetOldPositionY();

		angle.y += deltaX * mouseSensitivity * elapsedTime;
		angle.x += deltaY * mouseSensitivity * elapsedTime;
	}

	//! �}�E�X�̍X�V����
	mouse.Update();

	return true;

#else

	return false;

#endif // !_DEBUG
}

// �J�����̈ʒu�̕ύX�X�V����
bool CameraController::UpdateTargetChange(float elapsedTime)
{
	//! �^�[�Q�b�g�̈ʒu��ς��Ȃ��Ȃ�A�������~�߂�
	if (!targetChange)
		return false;

	//! �o�ߎ��Ԃ��v��
	targetChangeCurrentTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = targetChangeCurrentTime / targetChangeTime;

	switch (targetChangeEasing)
	{
	case TargetChangeEasing::Linear:
	{
		//! �^�[�Q�b�g�ύX
		target.x = Easing::Linear(startTargetChange.x, endTargetChange.x, t);
		target.y = Easing::Linear(startTargetChange.y, endTargetChange.y, t);
		target.z = Easing::Linear(startTargetChange.z, endTargetChange.z, t);
	}
	break;
	case TargetChangeEasing::EaseIn:
	{
		//! �^�[�Q�b�g�ύX
		target.x = Easing::EaseIn(startTargetChange.x, endTargetChange.x, t);
		target.y = Easing::EaseIn(startTargetChange.y, endTargetChange.y, t);
		target.z = Easing::EaseIn(startTargetChange.z, endTargetChange.z, t);
	}
	break;
	case TargetChangeEasing::EaseOut:
	{
		//! �^�[�Q�b�g�ύX
		target.x = Easing::EaseOut(startTargetChange.x, endTargetChange.x, t);
		target.y = Easing::EaseOut(startTargetChange.y, endTargetChange.y, t);
		target.z = Easing::EaseOut(startTargetChange.z, endTargetChange.z, t);
	}
	break;
	default:
		break;
	}

	//! �������~�߂�
	if (t >= 1.0f)
		targetChange = false;

	return true;
}

//------------------------------------------------------------------------------------------//

// �J�����̊p�x�̕ύX�X�V����
bool CameraController::UpdateAngleChange(float elapsedTime)
{
	//! �p�x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!angleChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	angleChangeCurrentTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = angleChangeCurrentTime / angleChangeTime;

	switch (angleChangeEasing)
	{
	case AngleChangeEasing::Linear:
	{
		//! �p�x�ύX
		angle.x = Easing::Linear(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::Linear(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::Linear(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::EaseIn:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseIn(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseIn(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseIn(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::EaseOut:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseOut(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseOut(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	default:
		break;
	}

	//! �������~�߂�
	if (t >= 1.0f)
		angleChange = false;

	return true;
}

//------------------------------------------------------------------------------------------//

// �J�����͈̔͂̕ύX�X�V����
bool CameraController::UpdateRangeChange(float elapsedTime)
{
	//! �J�����͈̔͂�ς��Ȃ��Ȃ�A�������~�߂�
	if (!rangeChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	rangeChangeCurrentTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = rangeChangeCurrentTime / rangeChangeTime;

	switch (rangeChangeEasing)
	{
	case RangeChangeEasing::Linear:
	{
		//! �͈͕ύX
		range = Easing::Linear(startRangeChange, endRangeChange, t);
	}
	break;
	case RangeChangeEasing::EaseIn:
	{
		//! �͈͕ύX
		range = Easing::EaseIn(startRangeChange, endRangeChange, t);
	}
	break;
	case RangeChangeEasing::EaseOut:
	{
		//! �͈͕ύX
		range = Easing::EaseOut(startRangeChange, endRangeChange, t);
	}
	break;
	default:
		break;
	}

	// �l�������Ȃ珈�����Ƃ߂�
	if (t >= 1.0f)
		rangeChange = false;

	return true;
}

// �J�����V�F�C�N�X�V����
bool CameraController::UpdateCameraShake(float elapsedTime)
{
	if (cameraShake && cameraShakeTimer > 0.0f)
	{
		// �h�炷�傫����1���傫���Ȃ珈�����s��
		if (cameraShakePower.x > 0)
			target.x += (rand() % cameraShakePower.x * 2 + 1) * elapsedTime;
		if (cameraShakePower.y > 0)
			target.y += (rand() % cameraShakePower.y * 2 + 1) * elapsedTime;
		if (cameraShakePower.z > 0)
			target.z += (rand() % cameraShakePower.z * 2 + 1) * elapsedTime;

		cameraShakeTimer -= elapsedTime;

		return true;
	}
	else
		cameraShake = false;

	return false;
}

// �J�������[�r�[���ԏ���
bool CameraController::UpdateCameraMovieTimer(float elapsedTime)
{
	// �J���������[�r�[���Ȃ�
	if (cameraMovie && movieTime > 0.0f)
	{
		movieTime -= elapsedTime;

		return true;
	}
	else
	{
		cameraMovie = false;
		movieTime = 0;
		return false;
	}
}

// ���̍X�V�������~�߂�X�V����
bool CameraController::UpdateBossFinish(float elapsedTime)
{
	if (bossFinish && bossFinishTimer > 0.0f)
	{
		// �^�C�}�[�����炷
		bossFinishTimer -= elapsedTime;

		// �ݒ������
		if (!bossFinishSettings)
		{
			//! �t�F�[�h��ݒ�
			fade->SetFade(
				DirectX::XMFLOAT3(1, 0, 0),
				0.9f, 0.0f,
				0.5f, 0.2f);

			// �J�����V�F�C�N
			SetCameraShake(0.8f, DirectX::XMINT3(100, 100, 0));

			// ���ʉ�
			SoundManager::Instance().PlaySound("�{�X���j", 0.5f);

			bossFinishSettings = true;
		}

		return true;
	}
	else
	{
		// �ϐ���ݒ�
		bossFinish = false;
		bossFinishTimer = 0.0f;
		return false;
	}
}