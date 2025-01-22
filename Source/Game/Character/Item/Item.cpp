#include "Item.h"
#include "ItemManager.h"
#include "Graphics/Graphics.h"

Item::Item(ItemManager* manager)
	: manager(manager)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void Item::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifdef _DEBUG

	debugRenderer->DrawSphere(collisionPos, radius, DirectX::XMFLOAT4(1, 1, 1, 1));

#endif // DEBUG
}

// �j��
void Item::Destroy()
{
	manager->Remove(this);
}

// ���o�̏�ԍX�V����
void Item::UpdateItemState(float elapsedTime, bool updateDirectionTarget, bool rotation, float rotationSpeed)
{
	// �s��X�V����
	UpdateTransform();

	// �����蔻��̈ʒu��ݒ�
	CollisionPosSettings();

	if (updateDirectionTarget)
		// �^�[�Q�b�g�������X�V����
		UpdateDirectionTarget(elapsedTime, rotation, rotationSpeed);

	// �傫���ύX�X�V����
	UpdateScaleChange(elapsedTime);

	// �}�e���A���̐F�ύX�X�V����
	UpdateMaterialColorChange(elapsedTime);

	// �s�����x�ύX�X�V����
	UpdateEffectOpacityChange(elapsedTime);

	//�G�~�b�V�u�̐F�ύX�X�V����
	UpdateEmissiveColorChange(elapsedTime);

	//�G�~�b�V�u�̋����ύX�X�V����
	UpdateEmissiveStrengthChange(elapsedTime);
}

// �X�V�s�񏈗�
void Item::UpdateTransform()
{
	DirectX::XMVECTOR Front, Up, Right;

	// �O�x�N�g�����Z�o
	Front = DirectX::XMLoadFloat3(&direction);


	// ���̏�x�N�g�����Z�o
	Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	// �E�x�N�g�����Z�o
	Right = DirectX::XMVector3Cross(Up, Front);
	Right = DirectX::XMVector3Normalize(Right);


	// ��x�N�g�����Z�o
	Up = DirectX::XMVector3Cross(Front, Right);
	Up = DirectX::XMVector3Normalize(Up);


	// �v�Z���ʂ����o��
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	// �Z�o�������x�N�g������s����쐬
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0.0f;
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0.0f;
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0.0f;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

	// ���˕���
	this->direction = front;
}

// �����蔻��̈ʒu��ݒ�
void Item::CollisionPosSettings()
{
	collisionPos = {
		position.x + collisionOffset.x,
		position.y + collisionOffset.y,
		position.z + collisionOffset.z
	};
}

// �^�[�Q�b�g�������X�V����
void Item::UpdateDirectionTarget(float elapsedTime, bool rotation, float rotationSpeed)
{
	// �^�[�Q�b�g�����x�N�g�����v�Z
	DirectX::XMFLOAT3 targetDir;
	targetDir.x = targetPosition.x - position.x;
	targetDir.y = targetPosition.y - position.y;
	targetDir.z = targetPosition.z - position.z;

	// ���݂̕����x�N�g�������[�h
	DirectX::XMVECTOR currentDirVec = DirectX::XMLoadFloat3(&direction);

	// �^�[�Q�b�g�����x�N�g���𐳋K�����ă��[�h
	DirectX::XMVECTOR targetDirVec = DirectX::XMLoadFloat3(&targetDir);
	targetDirVec = DirectX::XMVector3Normalize(targetDirVec);

	// ���ʐ��`��ԁiSlerp�j�Ŋ��炩�ɉ�]����
	DirectX::XMVECTOR newDirVec = DirectX::XMVectorLerp(currentDirVec, targetDirVec, rotationSpeed * elapsedTime);

	// ���ʂ𐳋K�����Ċi�[
	newDirVec = DirectX::XMVector3Normalize(newDirVec);
	DirectX::XMStoreFloat3(&direction, newDirVec);

	//! ��]����Ȃ�
	if (rotation)
	{
		// �p�x����]���x�ōX�V
		angle += rotationSpeed * elapsedTime;

		// �V�����ʒu���v�Z (�v���C���[�𒆐S�ɉ~��`��)
		targetPosition.x = position.x + cosf(angle) * distance;
		targetPosition.z = position.z + sinf(angle) * distance;

		// Y���W�͌Œ�
		targetPosition.y = position.y;
	}
}

// �傫���X�V����
bool Item::UpdateScaleChange(float elapsedTime)
{
	//! �傫����ύX���Ȃ��Ȃ�A�������~�߂�
	if (!scaleChange)
		return false;

	//! �傫���ύX
	scale.x = UpdateScaleAxis(scale.x, scaleChangeSpeed.x, scaleChangeUp.x, toScaleChange.x, elapsedTime);
	scale.y = UpdateScaleAxis(scale.y, scaleChangeSpeed.y, scaleChangeUp.y, toScaleChange.y, elapsedTime);
	scale.z = UpdateScaleAxis(scale.z, scaleChangeSpeed.z, scaleChangeUp.z, toScaleChange.z, elapsedTime);


	// �傫���ύX���I������珈�����~�߂�
	if (scale.x == toScaleChange.x
		&& scale.y == toScaleChange.y
		&& scale.z == toScaleChange.z)
		scaleChange = false;

	return true;
}

// �P�ꎲ�̃X�P�[���ύX�X�V����
float Item::UpdateScaleAxis(float scale, float speed, bool scaleChangeUp, float toScaleChange, float elapsedTime)
{
	// �傫���ύX
	scale += (scaleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڕW�l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((scaleChangeUp && scale > toScaleChange) || (!scaleChangeUp && scale < toScaleChange))
		scale = toScaleChange;

	// �傫����Ԃ�
	return scale;
}

//! �}�e���A���̐F�ύX�X�V����
bool Item::UpdateMaterialColorChange(float elapsedTime)
{
	//! �}�e���A���̐F��ς��Ȃ��Ȃ�A�������~�߂�
	if (!materialColorChange)
		return false;


	//! �}�e���A���̐F��ύX
	materialColor.x = UpdateMaterialColorAxis(materialColor.x, materialColorChangeSpeed.x, materialColorChangeUp.x, toMaterialColorChange.x, elapsedTime);
	materialColor.y = UpdateMaterialColorAxis(materialColor.y, materialColorChangeSpeed.y, materialColorChangeUp.y, toMaterialColorChange.y, elapsedTime);
	materialColor.z = UpdateMaterialColorAxis(materialColor.z, materialColorChangeSpeed.z, materialColorChangeUp.z, toMaterialColorChange.z, elapsedTime);

	//! �}�e���A���̐F�ύX���I�������A�������~�߂�
	if (materialColor.x == toMaterialColorChange.x
		&& materialColor.y == toMaterialColorChange.y
		&& materialColor.z == toMaterialColorChange.z)
		materialColorChange = false;

	return true;
}

// �P�ꎲ�̃}�e���A���̐F�ύX�X�V����
float Item::UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime)
{
	// �}�e���A���̐F�ύX
	materialColor += (materialColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((materialColorChangeUp && materialColor > toMaterialColorChange) || (!materialColorChangeUp && materialColor < toMaterialColorChange))
	{
		materialColor = toMaterialColorChange;
	}

	// �}�e���A���̐F��Ԃ�
	return materialColor;
}

//! �s�����x�ύX�X�V����
bool Item::UpdateEffectOpacityChange(float elapsedTime)
{
	//! �s�����x��ύX���Ȃ��Ȃ�A�������~�߂�
	if (!opacityChange)
		return false;


	//! �s�����x��ύX
	opacity += (opacityChangeUp ? 1 : -1) * opacityChangeSpeed * elapsedTime;

	//! �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((opacityChangeUp && opacity > toOpacityChange) || (!opacityChangeUp && opacity < toOpacityChange))
		opacity = toOpacityChange;

	//! �s�����x�̕ύX���I�������
	if (opacity == toOpacityChange)
		opacityChange = false;

	return true;
}

//!�G�~�b�V�u�̐F�ύX�X�V����
bool Item::UpdateEmissiveColorChange(float elapsedTime)
{
	//!�G�~�b�V�u�̐F��ς��Ȃ��Ȃ�A�������~�߂�
	if (!emissiveColorChange)
		return false;


	//!�G�~�b�V�u�̐F��ύX
	emissiveColor.x = UpdateEmissiveColorAxis(emissiveColor.x, emissiveColorChangeSpeed.x, emissiveColorChangeUp.x, toEmissiveColorChange.x, elapsedTime);
	emissiveColor.y = UpdateEmissiveColorAxis(emissiveColor.y, emissiveColorChangeSpeed.y, emissiveColorChangeUp.y, toEmissiveColorChange.y, elapsedTime);
	emissiveColor.z = UpdateEmissiveColorAxis(emissiveColor.z, emissiveColorChangeSpeed.z, emissiveColorChangeUp.z, toEmissiveColorChange.z, elapsedTime);

	//!�G�~�b�V�u�̐F�ύX���I�������A�������~�߂�
	if (emissiveColor.x == toEmissiveColorChange.x
		&& emissiveColor.y == toEmissiveColorChange.y
		&& emissiveColor.z == toEmissiveColorChange.z)
		emissiveColorChange = false;

	return true;
}

// �P�ꎲ�̃G�~�V�b�u�̐F�ύX�X�V����
float Item::UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime)
{
	//�G�~�b�V�u�̐F�ύX
	emissiveColor += (emissiveColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((emissiveColorChangeUp && emissiveColor > toEmissiveColorChange) || (!emissiveColorChangeUp && emissiveColor < toEmissiveColorChange))
	{
		emissiveColor = toEmissiveColorChange;
	}

	//�G�~�b�V�u�̐F��Ԃ�
	return emissiveColor;
}

//! �G�~�b�V�u�̋����ύX�X�V����
bool Item::UpdateEmissiveStrengthChange(float elapsedTime)
{
	//!�G�~�b�V�u�̋�����ς��Ȃ��Ȃ�A�������~�߂�
	if (!emissiveStrengthChange)
		return false;

	//!�G�~�b�V�u�̋�����ύX
	emissiveStrength += (emissiveStrengthChangeUp ? 1 : -1) * emissiveStrengthChangeSpeed * elapsedTime;

	//! �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((emissiveStrengthChangeUp && emissiveStrength > toEmissiveStrengthChange) || (!emissiveStrengthChangeUp && emissiveStrength < toEmissiveStrengthChange))
		emissiveStrength = toEmissiveStrengthChange;

	//!�G�~�b�V�u�̋����̕ύX���I�������A�������~�߂�
	if (emissiveStrength == toEmissiveStrengthChange)
		emissiveStrengthChange = false;

	return true;
}


