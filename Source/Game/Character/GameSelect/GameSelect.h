#pragma once

#include "Graphics/Shader.h"
#include "Game/Character/Character.h"
#include "Graphics/Graphics.h"

// �A�C�e��
class GameSelect : public Character
{
public:
	GameSelect() {}
	~GameSelect() override {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2D�X�v���C�g�̕`�揈��
	virtual void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) {};

	// �f�o�b�O�pGUI�`��
	virtual void DrawDebugGUI() {};

	// �f�o�b�O�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	void Destroy();
};