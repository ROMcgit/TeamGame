#include "Stage.h"

//! ��ԍX�V����
void Stage::UpdateStageState(float elapsedTime)
{
	// �s��X�V����
	UpdateTransform();

	// �s�����x�ύX�X�V����
	UpdateEffectOpacityChange(elapsedTime);

	// �G�~�b�V�u�̐F�ύX�X�V����
	UpdateEmissiveColorChange(elapsedTime);

	// �G�~�b�V�u�̋����ύX�X�V����
	UpdateEmissiveStrengthChange(elapsedTime);
}

// �s��X�V����
void Stage::UpdateTransform()
{
	// �ȑO�̕ϊ��s���ۑ�
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

// �}�e���A���̐F�ύX�X�V����
bool Stage::UpdateMaterialColorChange(float elapsedTime)
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
float Stage::UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime)
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
bool Stage::UpdateEffectOpacityChange(float elapsedTime)
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

//! �G�~�b�V�u�̐F�ύX�X�V����
bool Stage::UpdateEmissiveColorChange(float elapsedTime)
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
float Stage::UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime)
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

// �G�~�b�V�u�̋����ύX�X�V����
bool Stage::UpdateEmissiveStrengthChange(float elapsedTime)
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
