#include "Game/Stage/G3_StageSoratobuHusenWari.h"
#include <imgui.h>
#include "Game/Scene/G3_SoratobuHusenWari.h"

// コンストラクタ
G3_StageSoratobuHusenWari::G3_StageSoratobuHusenWari()
{
	// ステージモデルを読み込み
	model = std::make_unique <Model>("Data/Model/Stage/3.SoratobuHusenWari/StageSoratobuHusenWari.mdl");

	position.y = 13.0f;

	scale.x = scale.z = 0.5f;
	scale.y = 0.2f;
}

G3_StageSoratobuHusenWari::~G3_StageSoratobuHusenWari()
{
}

// 更新処理
void G3_StageSoratobuHusenWari::Update(float elapsedTime)
{
	// ステージの状態更新処理
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	// 位置更新処理
	UpdatePosition(elapsedTime);

	//レイキャストようにモデル空間行列にするために単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// 描画処理
void G3_StageSoratobuHusenWari::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// 表示用のためワールド行列を更新する
	model->UpdateTransform(transform);

	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model.get());
}

// レイキャスト
bool G3_StageSoratobuHusenWari::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
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

// デバッグGUI
void G3_StageSoratobuHusenWari::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"ステージ空飛ぶ風船割り"))
	{
		// 位置
		ImGui::DragFloat3(u8"位置", &position.x);

		// 回転
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::DragFloat3("角度", &a.x, 0.01f);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);

		// スケール
		ImGui::DragFloat3("大きさ", &scale.x, 0.01f);

		ImGui::TreePop();
	}
}

// 位置更新処理
void G3_StageSoratobuHusenWari::UpdatePosition(float elapsedTime)
{
	float speed = 0;
	if (G3_SoratobuHusenWari::gameTimer < 30.0f)
		speed = -5.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
		speed = -10.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
		speed = -15.0f;
	else
		speed = -20.0f;

	position.z += speed * elapsedTime;

	if (position.z < -80.0f)
		position.z = 200.0f;
}
