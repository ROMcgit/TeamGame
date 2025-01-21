#include "CameraTarget.h"
#include <imgui.h>
#include "Game/Camera/CameraController.h"

// コンストラクタ
CameraTarget::CameraTarget(DirectorManager* manager)
	: Director(manager)
{
	model = std::make_unique<Model>("Data/Model/CameraTarget/CameraTarget.mdl");

	// スケール
	scale.x = scale.y = scale.z = 0.01f;

	// 当たり判定
	radius = 0.0f;

	// アニメーション
	model->PlayAnimation(0, true);
}

// デストラクタ
CameraTarget::~CameraTarget()
{
}

// 更新処理
void CameraTarget::Update(float elapsedTime)
{
	if(!CameraController::noMoveDebugCamera)
		// 演出の状態更新処理
		UpdateDirectorState(elapsedTime, true, true, 2.0f);
	else
		// 演出の状態更新処理
		UpdateDirectorState(elapsedTime, true, false);

	// ターゲットの位置の状態更新処理
	UpdateTargetState();

	// モデル行列更新
	model->UpdateTransform(transform);

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);
}

// 描画処理
void CameraTarget::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	if(opacity != 0.0f)
		shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// デバッグ用GUI描画
void CameraTarget::DrawDebugGUI()
{
	if (ImGui::TreeNode("CameraTarget"))
	{
		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform"))
		{
			// 位置
			ImGui::DragFloat3("Position", &position.x, 0.01f);
			// 当たり判定の位置
			ImGui::DragFloat3("CollisionPos", &collisionPos.x, 0.01f);
			// 回転
			ImGui::DragFloat3("Direction", &direction.x, 0.01f);
			// スケール
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		}

		ImGui::TreePop();
	}
}

// 状態設定
void CameraTarget::SetState(const DirectX::XMFLOAT3 direction,
	const DirectX::XMFLOAT3& position,
	float moveSpeed)
{
	{
		this->direction = direction;
		this->position = position;
		this->moveSpeed = moveSpeed;

		UpdateTransform();
	}
}

// ターゲットの位置の状態更新処理
bool CameraTarget::UpdateTargetState()
{
	//! 位置を設定
	position = CameraController::target;

	//! デバッグカメラじゃないなら、ここで処理をとめる
	if (!CameraController::debugCamera) 
	{
		//! 不透明度を変更
		SetOpacityChange(0.0f, 2.0f);
		return false;
	}
	else if(opacity == 0.0f)
		//! 不透明度を変更
		SetOpacityChange(1.0f, 2.0f);

	//! デバッグカメラで移動できるなら
	if(!CameraController::noMoveDebugCamera)
	{
		//! 不透明度を変更
		SetOpacityChange(1.0f, 2.0f);

		//! エミッシブの強さを変更
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 1), 0.0f, 2.0f);
	}
	else if(CameraController::noMoveDebugCamera)
	{
		//! 不透明度を変更
		SetOpacityChange(0.6f, 2.0f);

		//! エミッシブの強さを変更
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 1), 0.5f, 2.0f);
	}

	return true;
}