#include "StageMain.h"

//static Stage* instance = nullptr;

// インスタンス取得
//Stage& Stage::Instance()
//{
//	return *instance;
//}

// コンストラクタ
//Stage::Stage()
StageMain::StageMain()
{
	//instance = this;

	scale.x = scale.y = scale.z = 0.01f;

	// ステージモデルを読み込み
	model = new Model("Data/Model/Stage/Field.mdl");
}

//Stage::~Stage()
StageMain::~StageMain()
{
	// ステージモデルを破棄
	delete model;
}

// 更新処理
//void Stage::Update(float elapsedTime)
void StageMain::Update(float elapsedTime)
{
	UpdateTransform();

	//レイキャストようにモデル空間行列にするために単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// 描画処理
//void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// 表示用のためワールド行列を更新する
	model->UpdateTransform(transform);

	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model);
}

//bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
}

void StageMain::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
