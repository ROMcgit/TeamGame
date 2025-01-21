#include "Camera.h"
#include "Graphics/ShaderSetting.h"
#include "Graphics/Graphics.h"
#include <imgui.h>

//! ポストエフェクト
Camera::CBPostEffectParam Camera::postEffect = {};
Microsoft::WRL::ComPtr<ID3D11Buffer> Camera::CBPostEffect;

//----------------------------------------------------------//

//! ポストエフェクトの数値をいじれるようにする
bool Camera::postEffectControll = false;
//! ポストエフェクトのステータスを元に戻す
bool Camera::postEffectReset = false;

//! ポストエフェクトのステータスを変更するか
bool Camera::postEffectChange = false;

//----------------------------------------------------------//

//! ここまでコンストラクトを変える
float Camera::toContrastChange = 0.0f;
//! コンストラクトの値を変える速度
float Camera::contrastChangeSpeed = 0.0f;
//! コンストラクトの値を上昇させるか
bool Camera::contrastUp = 0.0f;

//----------------------------------------------------------//

//! ここまでサチュレーション(色の彩度)を変える
float Camera::toSaturationChange = 0.0f;
//! サチュレーションの値を変える速度
float Camera::saturationChangeSpeed = 0.0f;
//! サチュレーションを上昇させるか
bool Camera::saturationUp = false;

//----------------------------------------------------------//

//! ここまでカラーフィルター(色フィルター)を変える
DirectX::XMFLOAT3 Camera::toColorFilterChange = { 0, 0, 0 };
//! カラーフィルターの値を変える速度
DirectX::XMFLOAT3 Camera::colorFilterChangeSpeed = { 0, 0, 0 };
//! カラーフィルターを上昇させるか
Camera::ColorFilterUp Camera::colorFilterUp = { false, false, false };

//----------------------------------------------------------//

//! ここまでクロマティックアベレーション(色収差(色ズレ))を変える
float Camera::toChromaticAberrationChange = 0.0f;
//! クロマティックアベレーションの値を変える速度
float Camera::chromaticAberrationChangeSpeed = 0.0f;
//! クロマティックアベレーションを上昇させるか
bool Camera::chromaticAberrationUp = false;


// ポストエフェクトを生成
void Camera::CreatePostEffect()
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	// posteffect構造体の内容でコンスタントバッファ更新
	dc->UpdateSubresource(CBPostEffect.Get(), 0, 0, &postEffect, 0, 0);

	// ピクセルシェーダーSlot5に設定
	dc->PSSetConstantBuffers(5, 1, CBPostEffect.GetAddressOf());
}

// ポストエフェクトのステータスを設定
void Camera::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3 ColorFilter, float chromaticAberration)
{
	// パラメータ初期化
	if (!postEffectControll)
	{
		postEffect.contrast = contrast;            // コンストラクト
		postEffect.saturation = saturation;          // サチュレーション
		postEffect.colorFilter = ColorFilter;         // カラーフィルター
		postEffect.chromaticAberration = chromaticAberration; // クロマティックアベレーション
	}
}

// コンストラクタ
Camera::Camera()
{
	// ポストエフェクト初期化
	ShaderSetting::CreateConstantBuffer(
		Graphics::Instance().GetDevice(),
		sizeof(CBPostEffectParam),
		CBPostEffect.GetAddressOf());

	// パラメータ初期化
	postEffect.contrast = 1.3f;
	postEffect.saturation = 0.7f;
	postEffect.colorFilter = { 0.9f, 1.0f, 1.05f };
	postEffect.chromaticAberration = 0.01f;
}

// 更新処理
void Camera::Update(float elapsedTime)
{
	// ポストエフェクトのステータスを元に戻す
	UpdatepostEffectReset();

	// ポストエフェクトのステータス変更処理
	UpdatePostEffectStatusChange(elapsedTime);
}

// デバッグGUI描画
void Camera::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::TreeNode("Camera"))
	{
		ImGui::InputFloat3("Eye", &eye.x);
		ImGui::InputFloat3("Focus", &focus.x);
		ImGui::InputFloat3("Up", &up.x);

		if (ImGui::CollapsingHeader("PostEffect"))
		{
			//! ゲーム中に数値をいじれるようにする
			ImGui::Checkbox("PostEffectControll", &postEffectControll);
			//! 数値をリセットする
			ImGui::Checkbox("PostEffectReset", &postEffectReset);

			// コンストラクト(数値が大きいほど、明るい部分はより明るく、暗い部分はより暗くなる)
			ImGui::DragFloat(u8"コンストラクト", &postEffect.contrast, 0.01f);
			// サチュレーション(色の彩度)
			ImGui::DragFloat(u8"彩度", &postEffect.saturation, 0.01f);
			// カラーフィルター(色フィルター)
			ImGui::DragFloat3(u8"色フィルター", &postEffect.colorFilter.x, 0.01f);
			// クロマティックアベレーション(色収差(色ズレ))
			ImGui::DragFloat(u8"色ズレ", &postEffect.chromaticAberration, 0.01f);
		}

		ImGui::TreePop();
	}
}

// 指定方向を向く
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	// 視点、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = { eye.x,eye.y,eye.z };
	DirectX::XMVECTOR Focus = { focus.x,focus.y,focus.z };
	DirectX::XMVECTOR Up = { up.x,up.y,up.z };
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	// ビューを逆行列化し、ワールド行列に戻す
	DirectX::CXMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	// カメラの方向を取り出す
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	// 視点、注視点を保存
	this->eye = eye;
	this->focus = focus;
}

// パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// 画角、画面比率、クリップ距離からプロジェクション行列を作成
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}

// ポストエフェクトのステータス変更処理
bool Camera::UpdatePostEffectStatusChange(float elapsedTime)
{
	//! ポストエフェクトのステータスを変更しないなら、処理を止める
	if (!postEffectChange)
		return false;

	//! コンストラクトの値を変える
	postEffect.contrast += (contrastUp ? 1 : -1) * (contrastChangeSpeed * elapsedTime);
	// 目標値を超えた場合、目標値にする
	if ((contrastUp && postEffect.contrast > toContrastChange) || (!contrastUp && postEffect.contrast < toContrastChange))
		postEffect.contrast = toContrastChange;

	//! サチュレーションの値を変える
	postEffect.saturation += (saturationUp ? 1 : -1) * (contrastChangeSpeed * elapsedTime);
	// 目標値を超えた場合、目標値にする
	if ((saturationUp && postEffect.saturation > toSaturationChange) || (!saturationUp && postEffect.saturation < toSaturationChange))
		postEffect.saturation = toSaturationChange;

	//! カラーフィルターの値を変える速度
	postEffect.colorFilter.x = UpdateColorComponent(postEffect.colorFilter.x, colorFilterChangeSpeed.x, colorFilterUp.x, toColorFilterChange.x, elapsedTime);
	postEffect.colorFilter.y = UpdateColorComponent(postEffect.colorFilter.y, colorFilterChangeSpeed.y, colorFilterUp.y, toColorFilterChange.y, elapsedTime);
	postEffect.colorFilter.z = UpdateColorComponent(postEffect.colorFilter.z, colorFilterChangeSpeed.z, colorFilterUp.z, toColorFilterChange.z, elapsedTime);

	//! クロマティックアベレーションの値を変える
	postEffect.chromaticAberration += (chromaticAberrationUp ? 1 : -1) * (chromaticAberrationChangeSpeed * elapsedTime);
	// 目標値を超えた場合、目指す値にする
	if ((chromaticAberrationUp && postEffect.chromaticAberration > toChromaticAberrationChange) || (!chromaticAberrationUp && postEffect.chromaticAberration < toChromaticAberrationChange))
		postEffect.chromaticAberration = toChromaticAberrationChange;

	//! 目指す値になったら処理を止める
	if (postEffect.contrast == toContrastChange
		&& postEffect.saturation == toSaturationChange
		&& postEffect.colorFilter.x == toColorFilterChange.x
		&& postEffect.colorFilter.y == toColorFilterChange.y
		&& postEffect.colorFilter.z == toColorFilterChange.z
		&& postEffect.chromaticAberration == toChromaticAberrationChange)
		postEffectChange = false;

	return true;
}

// カラーフィルターの変更処理
float Camera::UpdateColorComponent(float colorFilter, float speed, float colorFilterChangeUp, float toColorFilterChange, float elapsedTime)
{
	// カラーフィルターを変更
	colorFilter += (colorFilterChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((colorFilterChangeUp && colorFilter > toColorFilterChange) || (!colorFilterChangeUp && colorFilter < toColorFilterChange))
		colorFilter = toColorFilterChange;

	// カラーフィルターを返す
	return colorFilter;
}

// ポストエフェクトのステータスを元に戻す
bool Camera::UpdatepostEffectReset()
{
	//! リセットしたなら処理を止める
	if (postEffectReset)
		return false;

	postEffect.contrast = 1.3f;                  // コンストラクト
	postEffect.saturation = 0.7f;                  // サチュレーション
	postEffect.colorFilter = { 0.9f, 1.0f, 1.05f }; // カラーフィルター
	postEffect.chromaticAberration = 0.01f;                 // クロマティックアベレーション

	postEffectReset = true; // ポストエフェクトのステータスをリセットした

	return true;
}
