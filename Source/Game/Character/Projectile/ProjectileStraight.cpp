#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileManager.h"

// コンストラクタ
//ProjectileStraight::ProjectileStraight()
//{
//
//}

// コンストラクタ
ProjectileStraight::ProjectileStraight(ProjectileManager* manager) : Projectile(manager)
{
	//model = new Model("./Data/Model/SpikeBall/SpikeBall.mdl");
	model = new Model("./Data/Model/Sword/Sword.mdl");

	// 表示サイズを調整
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 6.0f;
}

// デストラクタ
ProjectileStraight::~ProjectileStraight()
{
	delete model;
}

// 更新処理
void ProjectileStraight::Update(float elapsedTime)
{
	// 寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0)
	{
		// 自分を削除
		Destroy();
	}


	// 移動
	float speed = this->speed * elapsedTime;

	// 位置を更新
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;

	// オブジェクト行列を更新
	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

// 発射
void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}