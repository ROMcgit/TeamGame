#include "Game/Stage/StageGameSelect.h"
#include <imgui.h>

// コンストラクタ
StageGameSelect::StageGameSelect()
{
	// ステージモデルを読み込み
	model = std::make_unique <Model>("Data/Model/Stage/StageGameSelect/StageGameSelect.mdl");

	position.y = -13.5f;

	scale.x = scale.y =  scale.z = 0.3f;
}

StageGameSelect::~StageGameSelect()
{
}

// 更新処理
void StageGameSelect::Update(float elapsedTime)
{
	// 行列更新
	UpdateTransform();

	//レイキャストようにモデル空間行列にするために単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// 描画処理
void StageGameSelect::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// 表示用のためワールド行列を更新する
	model->UpdateTransform(transform);

	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model.get());
}

// レイキャスト
bool StageGameSelect::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	// 前回のワールド行列と逆行列を求める
	DirectX::XMMATRIX oldWorld = DirectX::XMLoadFloat4x4(&oldTransform);
	DirectX::XMMATRIX invOldWorld = DirectX::XMMatrixInverse(nullptr, oldWorld);


	// 前回のローカル空間でのレイに変換
	DirectX::XMVECTOR rayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR rayEnd = DirectX::XMLoadFloat3(&end);

	DirectX::XMVECTOR localStartVec = DirectX::XMVector3TransformCoord(rayStart, invOldWorld);
	DirectX::XMVECTOR localEndVec = DirectX::XMVector3TransformCoord(rayEnd, invOldWorld);

	DirectX::XMFLOAT3 localStart, localEnd;
	DirectX::XMStoreFloat3(&localStart, localStartVec);
	DirectX::XMStoreFloat3(&localEnd, localEndVec);


	// ローカル空間でのレイとの交点を求める
	HitResult localHit;
	if (Collision::IntersectRayVsModel(localStart, localEnd, model.get(), localHit))
	{
		// 前回のローカル空間から今回のワールド空間へ変換
		// 前回から今回のかけて変更された内容が乗っているオブジェクトに反映される。
		// レイの始点と終点から交点の位置を計算
		DirectX::XMVECTOR localRayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(localEndVec, localStartVec));
		DirectX::XMVECTOR localHitPointVec = DirectX::XMVectorAdd(localStartVec, DirectX::XMVectorScale(localRayDir, localHit.distance));

		// 前回のローカル空間から今回のワールド空間へ変換
		DirectX::XMMATRIX currentWorld = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMVECTOR worldHitPointVec = DirectX::XMVector3TransformCoord(localHitPointVec, currentWorld);

		// 交点の位置を取得
		DirectX::XMFLOAT3 worldHitPoint;
		DirectX::XMStoreFloat3(&worldHitPoint, worldHitPointVec);
		hit.distance = localHit.distance;
		hit.position = worldHitPoint; // 交点の位置をヒット結果に設定

		// 法線の変換
		DirectX::XMVECTOR localNormalVec = DirectX::XMLoadFloat3(&localHit.normal);
		DirectX::XMVECTOR worldNormalVec = DirectX::XMVector3TransformNormal(localNormalVec, currentWorld);
		DirectX::XMStoreFloat3(&hit.normal, worldNormalVec);

		// 回転差分を算出
		hit.rotation.x = angle.x;
		hit.rotation.y = angle.y;
		hit.rotation.z = angle.z;

		return true;
	}
	return false;
}

// 行列更新処理
void StageGameSelect::UpdateTransform()
{
	// 以前の変換行列を保存
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}


// デバッグ用GUI描画
void StageGameSelect::DrawDebugGUI()
{
	if (ImGui::TreeNode("StageGameSelect"))
	{
		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform"))
		{
			// 位置
			ImGui::DragFloat3("Position", &position.x, 0.01f);

			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::DragFloat3("Scale", &scale.x, 0.001f);
		}
		ImGui::TreePop();
	}
}