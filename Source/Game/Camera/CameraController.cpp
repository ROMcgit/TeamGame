#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include "Audio/SoundManager.h"
#include "Graphics/Fade.h"
#include <imgui.h>

//! �^�[�Q�b�g
DirectX::XMFLOAT3 CameraController::target = { 0, 0, 0 };

//! �^�[�Q�b�g�̈ʒu��ϕς��邩
bool CameraController::targetChange = false;
//! �����܂Ń^�[�Q�b�g�̈ʒu��ς���
DirectX::XMFLOAT3 CameraController::toTargetChange = { 0, 0, 0 };
//! �^�[�Q�b�g�̈ʒu��ς��鑬��
DirectX::XMFLOAT3 CameraController::targetChangeSpeed = { 0, 0, 0 };
//! �^�[�Q�b�g�̈ʒu�𑝂₷��
CameraController::TargetChangeUp CameraController::targetChangeUp = { false, false, false };

/********************************************************************/

//! �p�x
DirectX::XMFLOAT3 CameraController::angle = { DirectX::XMConvertToRadians(26),0,0 };

//! �p�x��ς��邩
bool CameraController::angleChange = false;
//! �����܂Ŋp�x��ς���
DirectX::XMFLOAT3 CameraController::toAngleChange = { 0, 0, 0 };
//! �p�x��ς��鑬��
DirectX::XMFLOAT3 CameraController::angleChangeSpeed = { 0, 0, 0 };
//! �p�x�𑝂₷��
CameraController::AngleChangeUp CameraController::angleChangeUp = { false, false, false };

/********************************************************************/

//! �J�����͈̔�
float CameraController::range = 16.0f;

//! �J�����͈̔͂�ς��邩
bool CameraController::rangeChange = false;
//! �����܂ŃJ�����͈̔͂�ς���
float CameraController::toRangeChange = 0.0f;
//! �J�����͈̔͂�ς��鑬��
float CameraController::rangeChangeSpeed = 0.0f;
//! �J�����͈̔͂𑝂₷��
bool CameraController::rangeChangeUp = false;

/********************************************************************/

//! �J������h�炷��
bool CameraController::cameraShake = false;
//! �h�炷����
float CameraController::cameraShakeTimer = 0.0f;
//! �h�炷�傫��
DirectX::XMINT3 CameraController::cameraShakePower = { 0, 0, 0 };


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

	// �t�F�[�h�𐶐�
	fade = std::make_unique<Fade>();

	//SoundManager::Instance().LoadSound("�{�X���j", "Data/Audio/Sound/Enemy/Defeat.wav");
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
	UpdateCameraTarget(elapsedTime);
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

// �J�����̃^�[�Q�b�g�X�V����
void CameraController::UpdateCameraTarget(float elapsedTime)
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

// �J�������f�o�b�O
bool CameraController::UpdateDebugCamera(float elapsedTime)
{
#ifdef _DEBUG

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

#endif // !_DEBUG

	return true;
}

// �J�����̈ʒu�̕ύX�X�V����
bool CameraController::UpdateTargetChange(float elapsedTime)
{
	//! �^�[�Q�b�g�̈ʒu��ς��Ȃ��Ȃ�A�������~�߂�
	if (!targetChange)
		return false;

	//! �^�[�Q�b�g�̈ʒu��ς���
	angle.x = UpdateAngleAxis(target.x, targetChangeSpeed.x, targetChangeUp.x, toTargetChange.x, elapsedTime);
	angle.y = UpdateAngleAxis(target.y, targetChangeSpeed.y, targetChangeUp.y, toTargetChange.y, elapsedTime);
	angle.z = UpdateAngleAxis(target.z, targetChangeSpeed.z, targetChangeUp.z, toTargetChange.z, elapsedTime);

	//! �l�������Ȃ珈�����I���
	if (target.x == toTargetChange.x && target.y == toTargetChange.y && target.z == toTargetChange.z)
		targetChange = false;

	return true;
}

// �P�ꎲ�̃^�[�Q�b�g�ʒu�ύX����
float CameraController::UpdateTargetAxis(float target, float speed, bool targetChangeUp, float toTargetChangeChange, float elapsedTime)
{
	// �^�[�Q�b�g
	target += (targetChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((targetChangeUp && target > toTargetChangeChange) || (!targetChangeUp && target < toTargetChangeChange))
		target = toTargetChangeChange;

	// �p�x��Ԃ�
	return target;
}

//------------------------------------------------------------------------------------------//

// �J�����̊p�x�̕ύX�X�V����
bool CameraController::UpdateAngleChange(float elapsedTime)
{
	//! �p�x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!angleChange)
		return false;

	//! �p�x��ς���
	angle.x = UpdateAngleAxis(angle.x, angleChangeSpeed.x, angleChangeUp.x, toAngleChange.x, elapsedTime);
	angle.y = UpdateAngleAxis(angle.y, angleChangeSpeed.y, angleChangeUp.y, toAngleChange.y, elapsedTime);
	angle.z = UpdateAngleAxis(angle.z, angleChangeSpeed.z, angleChangeUp.z, toAngleChange.z, elapsedTime);

	//! �l�������Ȃ珈�����I���
	if (angle.x == toAngleChange.x && angle.y == toAngleChange.y && angle.z == toAngleChange.z)
		angleChange = false;

	return true;
}

// �P�ꎲ�̉�]�ύX����
float CameraController::UpdateAngleAxis(float angle, float speed, bool angleChangeUp, float toAngleChange, float elapsedTime)
{
	// �p�x��ύX
	angle += (angleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((angleChangeUp && angle > toAngleChange) || (!angleChangeUp && angle < toAngleChange))
		angle = toAngleChange;

	// �p�x��Ԃ�
	return angle;
}

//------------------------------------------------------------------------------------------//

// �J�����͈̔͂̕ύX�X�V����
bool CameraController::UpdateRangeChange(float elapsedTime)
{
	//! �J�����͈̔͂�ς��Ȃ��Ȃ�A�������~�߂�
	if (!rangeChange)
		return false;

	//! �J�����͈̔͂�ς���
	range += (rangeChangeUp ? 1 : -1) * rangeChangeSpeed * elapsedTime;

	// �ڕW�l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((rangeChangeUp && range > toRangeChange) || (!rangeChangeUp && range < toRangeChange))
		range = toRangeChange;

	// �l�������Ȃ珈�����Ƃ߂�
	if (range == toRangeChange)
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
			fade->SetFade(
				DirectX::XMFLOAT3(1, 0, 0),
				0.9f, 0.0f,
				2.0f, 0.2f);

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