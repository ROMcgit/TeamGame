#include "Game/Character/Projectile/ProjectileUnko.h"
#include "Game/Character/Projectile/ProjectileManager.h"

// コンストラクタ
ProjectileUnko::ProjectileUnko(ProjectileManager* manager) : Projectile(manager)
{
	model = new Model("./Data/Model/Monkey/0.mdl/Projectile_Unko.mdl");

	destoryEffect = std::make_unique <Effect>("Data/Effect/Effect/Hit.efk");

	// 表示サイズを調整
	scale.x = scale.y = scale.z = 0.023f;

	collisionOffset = { 0, 0.7f, 0 };

	radius = 0.8f;

	model->PlayAnimation(0, true); // アニメーション
}

// デストラクタ
ProjectileUnko::~ProjectileUnko()
{
	delete model;
}

// 更新処理
void ProjectileUnko::Update(float elapsedTime)
{
	// 当たり判定の位置設定
	CollisionPosSettings();

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// 寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0)
	{
		DirectX::XMFLOAT3 e = collisionPos;
		e.y += collisionPos.y - 0.2f;
		destoryEffect->Play(e, 0.5f);

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
void ProjectileUnko::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

// 発射
void ProjectileUnko::Launch(const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}