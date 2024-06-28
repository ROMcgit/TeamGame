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

	// ステージモデルを読み込み
	model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
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
	//
}

// 描画処理
//void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model);
}

//bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
}