#include "Game/Stage/G0_StageOnigokko.h"

// コンストラクタ
G0_StageOnigokko::G0_StageOnigokko()
{
	// ステージモデルを読み込み
	model = std::make_unique <Model>("Data/Model/Stage/0.StageOnigokko/StageOnigokko.mdl");

	scale.x = scale.z = 0.5f;
	scale.y = 0.1f;

}

G0_StageOnigokko::~G0_StageOnigokko()
{
}

// 更新処理
void G0_StageOnigokko::Update(float elapsedTime)
{
	// 行列更新
	UpdateTransform();

	//レイキャストようにモデル空間行列にするために単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// 描画処理
void G0_StageOnigokko::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// 表示用のためワールド行列を更新する
	model->UpdateTransform(transform);

	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model.get());
}

// レイキャスト
bool G0_StageOnigokko::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return false;
}

// 行列更新処理
void G0_StageOnigokko::UpdateTransform()
{
	// 以前の変換行列を保存
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
