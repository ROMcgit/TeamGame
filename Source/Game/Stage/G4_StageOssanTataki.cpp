#include "Game/Stage/G4_StageOssanTataki.h"
#include <imgui.h>

// コンストラクタ
G4_StageOssanTataki::G4_StageOssanTataki()
{
	// ステージモデルを読み込み
	model = std::make_unique <Model>("Data/Model/Stage/4.OssanTataki/StageOssanTataki.mdl");

	scale.x = scale.z = 0.5f;
	scale.y = 0.2f;
}

G4_StageOssanTataki::~G4_StageOssanTataki()
{
}

// 更新処理
void G4_StageOssanTataki::Update(float elapsedTime)
{
	// ステージの状態更新処理
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	//レイキャストようにモデル空間行列にするために単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// 描画処理
void G4_StageOssanTataki::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// 表示用のためワールド行列を更新する
	model->UpdateTransform(transform);

	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model.get());
}

// レイキャスト
bool G4_StageOssanTataki::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return false;
}

// デバッグGUI
void G4_StageOssanTataki::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"ステージおっさん叩き"))
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
