#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"
#include "Graphics/Fade.h"
#include "Graphics/Text.h"
#include "Graphics/Sprite.h"

// �v���C���[
class Player2_Sundome : public Character
{
public:
	Player2_Sundome();
	~Player2_Sundome() override;

	// �C���X�^���X�擾
	static Player2_Sundome& Instance();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// HP�Ȃǂ�UI�`��
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// �f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	//! ���E���h���擾
	int GetRound() { return round; }

protected:
	// ���n�������ɌĂ΂��
	void OnLanding() override;

	// �_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;

	// ���S�������ɌĂ΂��
	void OnDead() override;

private:

	// �ړ����͏���
	bool InputMove(float elapsedTime);

	/*! �����蔻�菈�� */

	// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	void CollisionPlayer2_SundomeVsEnemies();

/******************************************************************************************/

	/*! �s������ */

	// ���[�r�[�ҋ@�X�e�[�g�֑J��
	void TransitionMovieWaitState();

	// ���[�r�[�ҋ@�X�e�[�g�X�V����
	void UpdateMovieWaitState(float elapsedTime);

	// �ҋ@�X�e�[�g�֑J��
	void TransitionWaitState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateWaitState(float elapsedTime);

//-----------------------------------------------------------------//

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

	// �߂�X�e�[�g�֑J��
	void TransitionReturnState();

	// �߂�X�e�[�g�X�V����
	void UpdateReturnState(float elapsedTime);

//-----------------------------------------------------------------//

	// �_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	// �_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

//-----------------------------------------------------------------//

	// ���S�X�e�[�g�֑J��
	void TransitionDeathState();

	// ���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTimae);

private:

	enum class State
	{
		MovieWait,
		Wait,
		Move,
		Return,
		Jump,
		Damage,
		Death
	};

private:
	std::unique_ptr<Model> models[3];
	std::unique_ptr<Fade> fade;

	std::unique_ptr<Sprite> actionExplanation[2];

	bool isBrake = false;
	bool breakeSound = false;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	std::unique_ptr<Effect> hitEffect;

	State state = State::Wait;

	float playerAnimeCount = 0.0f;

	std::unique_ptr<Sprite> velocitySprite;
	std::unique_ptr<Sprite> velocitySpriteLimit;
	DirectX::XMFLOAT2 spritePos   = { 1170.0f, 640.0f };
	DirectX::XMFLOAT2 spriteScale = { 76.0f, -400.0f };

	std::unique_ptr<Text> velocityText;
	std::unique_ptr<Text> velocityTextSyosuten;
	DirectX::XMFLOAT2 velocityTextPos = { 1150.0f, 0.0f };

	struct VelocityLimit
	{
		float min = 0.0f;
		float max = 60.0f;
	}velocityLimit;

	bool velocityDown = false; // �����x�������邩
	float setVelocityX = 0.0f;

	float brake = 0;

	int round = 1;

	float boundTimer = 0.0f;
};