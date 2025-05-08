#include "Game/Stage/G5_StageAsibawatari_Trap.h"
#include <imgui.h>

// コンストラクタ
G5_StageAsibawatari_Trap::G5_StageAsibawatari_Trap()
{
	// ステージモデルを読み込み
	model = std::make_unique <Model>("Data/Model/Stage/5.Asibawatari/1.Trap/StageTrap.mdl");

	scale.x = scale.y = scale.z = 0.1f;

	opacity = 0.5f;
}

G5_StageAsibawatari_Trap::~G5_StageAsibawatari_Trap()
{
}

// 更新処理
void G5_StageAsibawatari_Trap::Update(float elapsedTime)
{
	noViewTime -= elapsedTime;

	// ステージの状態更新処理
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	//レイキャストようにモデル空間行列にするために単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);

	// 移動更新処理
	UpdateMove(elapsedTime);
}

// 描画処理
void G5_StageAsibawatari_Trap::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// 表示用のためワールド行列を更新する
	model->UpdateTransform(transform);

	// シェーダーにモデルを描画してもらう
	if(noViewTime <= 0.0f)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// レイキャスト
bool G5_StageAsibawatari_Trap::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return false;
}

// デバッグGUI
void G5_StageAsibawatari_Trap::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"ステージトラップ"))
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

// 移動処理
void G5_StageAsibawatari_Trap::UpdateMove(float elapsedTime)
{
	position.x -= moveSpeed * elapsedTime;

	if (position.x < -50.0f)
		Destroy();
}