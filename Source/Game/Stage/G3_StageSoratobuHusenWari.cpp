#include "Game/Stage/G3_StageSoratobuHusenWari.h"
#include <imgui.h>

// コンストラクタ
G3_StageSoratobuHusenWari::G3_StageSoratobuHusenWari()
{
	// ステージモデルを読み込み
	model = std::make_unique <Model>("Data/Model/Stage/3.SoratobuHusenWari/StageSoratobuHusenWari.mdl");

	scale.x = scale.y = scale.z = 0.5f;

}

G3_StageSoratobuHusenWari::~G3_StageSoratobuHusenWari()
{
}

// 更新処理
void G3_StageSoratobuHusenWari::Update(float elapsedTime)
{
	// ステージの状態更新処理
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

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
