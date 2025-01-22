#pragma once

#include "Graphics/Shader.h"
#include "Other/Collision.h"

// �X�e�[�W
class Stage
{
public:
	Stage() {}
	virtual ~Stage() {};

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// ���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	// �f�o�b�O�pGUI�`��
	virtual void DrawDebugGUI() {};

	/*****************************************************************************************************************************/
		/*! �Z�b�^�[ */

		// �}�e���A���̐F�̐ݒ�
	void SetMaterialColor(const DirectX::XMFLOAT3 materialColor) { this->materialColor = materialColor; }

	// �}�e���A���̐F�ύX�̐ݒ�
	void SetMaterialColorChange(DirectX::XMFLOAT3 toMaterialColorChange, DirectX::XMFLOAT3 materialColorChangeSpeed)
	{
		if (!this->materialColorChange)
		{
			this->materialColorChange = true;                     // �}�e���A���̐F��ς���
			this->toMaterialColorChange = toMaterialColorChange;    // �����܂Ń}�e���A���̐F��ς���
			this->materialColorChangeSpeed = materialColorChangeSpeed; // �}�e���A���̐F��ς��鑬��

			// �}�e���A���J���[X���ڎw���l��菬������
			if (materialColor.x < toMaterialColorChange.x)
				materialColorChangeUp.x = true;
			else
				materialColorChangeUp.x = false;

			// �}�e���A���J���[Y���ڎw���l��菬������
			if (materialColor.y < toMaterialColorChange.y)
				materialColorChangeUp.y = true;
			else
				materialColorChangeUp.y = false;

			// �}�e���A���J���[Z���ڎw���l��菬������
			if (materialColor.z < toMaterialColorChange.z)
				materialColorChangeUp.z = true;
			else
				materialColorChangeUp.z = false;
		}
	}

	// �s�����x��ݒ�
	void SetOpacity(float opacity) { this->opacity = opacity; }

	// �s�����x�ύX�����̐ݒ�
	void SetOpacityChange(float toOpacityChange, float opacityChangeSpeed)
	{
		if (!this->opacityChange)
		{
			this->opacityChange = true;               // �s�����x��ς���
			this->toOpacityChange = toOpacityChange;    // �����܂ŕs�����x��ς���
			this->opacityChangeSpeed = opacityChangeSpeed; // �s�����x��ς��鑬��

			// �s�����x���ڎw���l��菬������
			if (opacity < toOpacityChange)
				opacityChangeUp = true;
			else
				opacityChangeUp = false;
		}
	}

	//-----------------------------------------------------------------------------//

		// �G�~�b�V�u�̐F�̐ݒ�
	void SetEmissiveColor(const DirectX::XMFLOAT3 emissiveColor) { this->emissiveColor = emissiveColor; }

	// �G�~�b�V�u�̐F�ύX�̐ݒ�
	void SetEmissiveColorChange(DirectX::XMFLOAT3 toEmissiveColorChange, DirectX::XMFLOAT3 emissiveColorChangeSpeed)
	{
		if (!this->emissiveColorChange)
		{
			this->emissiveColorChange = true;                     //�G�~�b�V�u�̐F��ς���
			this->toEmissiveColorChange = toEmissiveColorChange;    // �����܂ŃG�~�V�b�u�̐F��ς���
			this->emissiveColorChangeSpeed = emissiveColorChangeSpeed; //�G�~�b�V�u�̐F��ς��鑬��

			//�G�~�b�V�u�J���[X���ڎw���l��菬������
			if (emissiveColor.x < toEmissiveColorChange.x)
				emissiveColorChangeUp.x = true;
			else
				emissiveColorChangeUp.x = false;

			//�G�~�b�V�u�J���[Y���ڎw���l��菬������
			if (emissiveColor.y < toEmissiveColorChange.y)
				emissiveColorChangeUp.y = true;
			else
				emissiveColorChangeUp.y = false;

			//�G�~�b�V�u�J���[Z���ڎw���l��菬������
			if (emissiveColor.z < toEmissiveColorChange.z)
				emissiveColorChangeUp.z = true;
			else
				emissiveColorChangeUp.z = false;
		}
	}

	// �G�~�b�V�u�̋����̐ݒ�
	void SetEmissiveStrength(float emissiveStrength) { this->emissiveStrength = emissiveStrength; }

	// �G�~�b�V�u�̋����ύX�̐ݒ�(�ݒ�̎��ɐF��ς����)
	void SetEmissiveStrengthChange(DirectX::XMFLOAT3 emissiveColor, float toEmissiveStrengthChange, float emissiveStrengthChangeSpeed)
	{
		if (!this->emissiveStrengthChange)
		{
			this->emissiveColor = emissiveColor;               //�G�~�b�V�u�̐F
			this->emissiveStrengthChange = true;                        //�G�~�b�V�u�̋�����ς���
			this->toEmissiveStrengthChange = toEmissiveStrengthChange;    // �����܂ŃG�~�b�V�u�̋�����ς���
			this->emissiveStrengthChangeSpeed = emissiveStrengthChangeSpeed; // �G�~�b�V�u�̋�����ς��鑬��

			//!�G�~�b�V�u�̋������ڎw���l��菬������
			if (emissiveStrength < toEmissiveStrengthChange)
				emissiveStrengthChangeUp = true;
			else
				emissiveStrengthChangeUp = false;
		}
	}

	//----------------------------------------------------------------------------------------------------//

		// ��ԍX�V����
	void UpdateStageState(float elapsedTime);

	// �s��X�V����
	void UpdateTransform();

	// �}�e���A���̐F�ύX�X�V����
	bool UpdateMaterialColorChange(float elapsedTime);

	// �P�ꎲ�̃}�e���A���̐F�ύX�X�V����
	float UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime);

	// �s�����x�ύX�X�V����
	bool UpdateEffectOpacityChange(float elapsedTime);

	// �G�~�b�V�u�̐F�ύX�X�V����
	bool UpdateEmissiveColorChange(float elapsedTime);

	// �P�ꎲ�̃G�~�V�b�u�̐F�ύX�X�V����
	float UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime);

	// �G�~�b�V�u�̋����ύX�X�V����
	bool UpdateEmissiveStrengthChange(float elapsedTime);

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT4X4 oldTransform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };

	/***************************************************************************************/
		/*! �}�e���A�� */
	DirectX::XMFLOAT3 materialColor = { 1,1,1 }; // �}�e���A���̐F
	bool materialColorChange = false;            // �}�e���A���̐F��ύX���邩
	//�}�e���A���̐F�𑝂₷��
	struct MaterialColorChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}materialColorChangeUp;  // �}�e���A���̐F�𑝂₷��

	DirectX::XMFLOAT3 toMaterialColorChange = { 0, 0, 0 }; // �����܂Ń}�e���A���̐F��ς���
	DirectX::XMFLOAT3 materialColorChangeSpeed = { 0, 0, 0 }; // �}�e���A���̐F��ς��鑬��

	//! �s�����x
	float opacity = 1.0f;    // �s�����x
	bool  opacityChange = false;   // �s�����x��ύX���邩
	bool  opacityChangeUp = false;   // �s�����x�𑝂₷��
	float toOpacityChange = 0.0f;    // �����܂ŕs�����x��ς��� 
	float opacityChangeSpeed = 0.0f; // �s�����x��ς��鑬�x

	/***************************************************************************************/
		/*!�G�~�b�V�u */
	DirectX::XMFLOAT3 emissiveColor = { 0.0f, 0.0f, 0.0f }; //�G�~�b�V�u�J���[
	bool emissiveColorChange = false; //�G�~�b�V�u�̐F��ς��邩
	//�G�~�b�V�u�̐F�𑝂₷��
	struct EmissiveColorChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}emissiveColorChangeUp;  //�G�~�b�V�u�̐F�𑝂₷��

	DirectX::XMFLOAT3 toEmissiveColorChange = { 0, 0, 0 }; // �����܂ŃG�~�V�b�u�̐F��ς���
	DirectX::XMFLOAT3 emissiveColorChangeSpeed = { 0, 0, 0 }; //�G�~�b�V�u�̐F��ς��鑬��

	//!�G�~�b�V�u�̋���
	float emissiveStrength = 0.0f;  //�G�~�b�V�u�̋���
	bool  emissiveStrengthChange = false; //�G�~�b�V�u�̋�����ς��邩
	bool  emissiveStrengthChangeUp = false; //�G�~�b�V�u�̋����𑝂₷��

	float toEmissiveStrengthChange = 0.0f;  //�@�����܂ŃG�~�V�b�u�̋�����ς���
	float emissiveStrengthChangeSpeed = 0.0f;  //�@�G�~�V�b�u�̋�����ς��鑬��
};