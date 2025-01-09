#include "Game/Character/Projectile/ProjectileHoming.h"

// コンストラクタ
ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
	: Projectile(manager)
{
	model = new Model("Data/Model/Sword/Sword.mdl");

	// モデルが小さいのでスケーリング
	scale.x = scale.y = scale.z = 6.0f;
}

// デストラクタ
ProjectileHoming::~ProjectileHoming()
{
	delete model;
}

// 更新処理
void ProjectileHoming::Update(float elapsedTime)
{
	// 寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0)
	{
		// 自分を削除
		Destroy();
	}

	// 移動
	{
		float moveSpeed = this->moveSpeed / 2 * elapsedTime;
		DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR directionVec = DirectX::XMLoadFloat3(&direction);
		positionVec = DirectX::XMVectorAdd(positionVec, DirectX::XMVectorScale(directionVec, moveSpeed));
		DirectX::XMStoreFloat3(&position, positionVec);
		position.x += direction.x * moveSpeed;
		position.y += direction.y * moveSpeed;
		position.z += direction.z * moveSpeed;
	}

	// 旋回
	{
		float turnSpeed = this->turnSpeed / 2 * elapsedTime;

		// ターゲットまでのベクトルを算出
		DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
		DirectX::XMVECTOR vecToTarget = DirectX::XMVectorSubtract(targetVec, positionVec);

		// ゼロベクトルでないなら回転処理
		DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(vecToTarget);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, LengthSq);
		if (lengthSq > 0.00001f)
		{
			// ターゲットまでのベクトルを単位ベクトル化
			vecToTarget = DirectX::XMVector3Normalize(vecToTarget);

			// 向いている方向ベクトルを算出
			DirectX::XMVECTOR directionVec = DirectX::XMLoadFloat3(&direction);

			// 前方方向ベクトルとターゲットまでのベクトルの内積(角度)を算出
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(directionVec, vecToTarget);

			float dot;
			DirectX::XMStoreFloat(&dot, Dot);

			// ２つの単位ベクトルの角度が小さきほど、
			// 1.0に近づくという性質を利用して回転速度を調整する

			float adjustedTurnSpeed = 1.0f - dot; // 内積が1.0に近いほど回転速度を小さくする
			if (adjustedTurnSpeed > turnSpeed) adjustedTurnSpeed = turnSpeed;

			// 回転行列を単位行列にリセット
			DirectX::XMMATRIX Rotation = DirectX::XMMatrixIdentity();

			// 回転角度があるなら回転処理をする
			if (dot < 0.99999f)
			{
				// 回転軸を算出
				DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(directionVec, vecToTarget);
				Axis = DirectX::XMVector3Normalize(Axis);

				// 回転軸と回転量から回転行列を算出
				DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, adjustedTurnSpeed);

				// 現在の行列を回転させる
				DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
				Transform = DirectX::XMMatrixMultiply(Transform, Rotation);
				DirectX::XMStoreFloat4x4(&transform, Transform);

				// 回転後の前方方向を取り出し、単位ベクトル化する
				directionVec = DirectX::XMVector3TransformNormal(directionVec, Rotation);
				directionVec = DirectX::XMVector3Normalize(directionVec);
				DirectX::XMStoreFloat3(&direction, directionVec);
			}
		}
	}

	// オブジェクト行列を更新
	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

// 発射
void ProjectileHoming::Launch(
	const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& position, 
	const DirectX::XMFLOAT3& target)
{
	{
		this->direction = direction;
		this->position = position;
		this->target = target;

		UpdateTransform();
	}
}