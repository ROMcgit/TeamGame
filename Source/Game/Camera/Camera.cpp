#include "Camera.h"
#include "Graphics/ShaderSetting.h"
#include "Graphics/Graphics.h"
#include "Other/Easing.h"
#include <imgui.h>

//! ポストエフェクト
Camera::CBPostEffectParam Camera::postEffect = {};
Microsoft::WRL::ComPtr<ID3D11Buffer> Camera::CBPostEffect;

//----------------------------------------------------------//

//! ポストエフェクトの数値をいじれるようにする
bool Camera::postEffectControll = false;
//! ポストエフェクトのステータスを元に戻す
bool Camera::postEffectReset = false;

//----------------------------------------------------------//

//! コントラストの値を変更するか
bool Camera::contrastChange = false;
//! コントラストの変更の開始の値
float Camera::startContrastChange = 0.0f;
//! ここまでコントラストを変える
float Camera::endContrastChange = 0.0f;
//! コントラストの値を変える時間
float Camera::contrastChangeTime = 0.0f;
//! コントラスト変更の経過時間
float Camera::contrastChangeCurrentTime = 0.0f;

//----------------------------------------------------------//

//! サチュレーションの値を変更するか
bool Camera::saturationChange = false;
//! サチュレーションの変更の開始の値
float Camera::startSaturationChange = 0.0f;
//! ここまでサチュレーション(色の彩度)を変える
float Camera::endSaturationChange = 0.0f;
//! サチュレーションの値を変える時間
float Camera::saturationChangeTime = 0.0f;
// !サチュレーション変更の経過時間
float Camera::saturationChangeCurrentTime = 0.0f;

//----------------------------------------------------------//

//! カラーフィルターの値を変更するか
bool Camera::colorFilterChange = false;
//! カラーフィルターの変更の開始の値
DirectX::XMFLOAT3 Camera::startColorFilterChange = { 0, 0, 0 };
//! ここまでカラーフィルター(色フィルター)を変える
DirectX::XMFLOAT3 Camera::endColorFilterChange = { 0, 0, 0 };
//! カラーフィルターの値を変える時間
float Camera::colorFilterChangeTime = 0.0f;
//! カラーフィルター変更の経過時間
float Camera::colorFilterChangeCurrentTime = 0.0f;

//----------------------------------------------------------//

//! クロマティックアベレーション(色収差(色ズレ))の値を変更するか
bool Camera::chromaticAberrationChange = false;
//! クロマティックアベレーションの変更の開始の値
float Camera::startChromaticAberrationChange = 0.0f;
//! ここまでクロマティックアベレーションを変える
float Camera::endChromaticAberrationChange = 0.0f;
//! クロマティックアベレーションの値を変える時間
float Camera::chromaticAberrationChangeTime = 0.0f;
//! クロマティックアベレーション変更の経過時間
float Camera::chromaticAberrationChangeCurrentTime = 0.0f;

// ポストエフェクトを生成
void Camera::CreatePostEffect()
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	// posteffect構造体の内容でコンスタントバッファ更新
	dc->UpdateSubresource(CBPostEffect.Get(), 0, 0, &postEffect, 0, 0);

	// ピクセルシェーダーSlot5に設定する
	dc->PSSetConstantBuffers(5, 1, CBPostEffect.GetAddressOf());
}

//! ポストエフェクトのステータスの設定(一回だけ)
void Camera::SetPostEffectStatusOnce(float contrast, float saturation, const DirectX::XMFLOAT3 colorFilter, float chromaticAberration)
{
	// パラメータ初期化
	if (!setPostEffectStatusOnce)
	{
		//! 変更処理を解除
		contrastChange = false; // コントラストの変更
		saturationChange = false; // サチュレーションの変更
		colorFilterChange = false; // カラーフィルターの変更
		chromaticAberrationChange = false; // クロマティックアベレーションの変更

		//! ポストエフェクト
		postEffect.contrast = contrast;            // コントラスト
		postEffect.saturation = saturation;          // サチュレーション
		postEffect.colorFilter = colorFilter;         // カラーフィルター
		postEffect.chromaticAberration = chromaticAberration; // クロマティックアベレーション

		setPostEffectStatusOnce = true; // ポストエフェクトのステータスの設定(一回だけ)をした
	}
}

// ポストエフェクトのステータスを設定
void Camera::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3 colorFilter, float chromaticAberration)
{
	//! パラメータの設定
	if (!postEffectControll)
	{
		postEffect.contrast = contrast;            // コントラスト
		postEffect.saturation = saturation;          // サチュレーション
		postEffect.colorFilter = colorFilter;         // カラーフィルター
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
	postEffect.contrast = 1.0f;
	postEffect.saturation = 0.8f;
	postEffect.colorFilter = { 1.2f, 1.3f, 1.35f };
	postEffect.chromaticAberration = 0.0f;
}

// 更新処理
void Camera::Update(float elapsedTime)
{
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

//! ポストエフェクトのステータス変更処理
void Camera::UpdatePostEffectStatusChange(float elapsedTime)
{
	//! コントラスト変更更新処理
	UpdateContrastChange(elapsedTime);

	//! サチュレーション変更更新処理
	UpdateSaturationChange(elapsedTime);

	//! カラーフィルター変更更新処理
	UpdateColorFilterChange(elapsedTime);

	//! クロマティックアベレーション変更更新処理
	UpdateChromaticAberrationChange(elapsedTime);
}

//! コントラスト変更更新処理
bool Camera::UpdateContrastChange(float elapsedTime)
{
	//! コントラストを変えないなら、処理を止める
	if (!contrastChange)
		return false;


	//! 経過時間を計測
	contrastChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = contrastChangeCurrentTime / contrastChangeTime;

	//! コントラストを変更する
	postEffect.contrast = Easing::Linear(startContrastChange, endContrastChange, t);

	//! 処理を止める
	if (t >= 1.0f)
		contrastChange = false;

	return true;
}

//! サチュレーション(色の彩度)変更更新処理
bool Camera::UpdateSaturationChange(float elapsedTime)
{
	//! サチュレーションを変えないなら、処理を止める
	if (!saturationChange)
		return false;


	//! 経過時間を計測
	saturationChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = saturationChangeCurrentTime / saturationChangeTime;

	//! サチュレーションを変更する
	postEffect.saturation = Easing::Linear(startSaturationChange, endSaturationChange, t);

	//! 処理を止める
	if (t >= 1.0f)
		saturationChange = false;

	return true;
}

//! カラーフィルター変更更新処理
bool Camera::UpdateColorFilterChange(float elapsedTime)
{
	//! カラーフィルターを変えないなら、処理を止める
	if (!colorFilterChange)
		return false;


	//! 経過時間を計測
	colorFilterChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = colorFilterChangeCurrentTime / colorFilterChangeTime;

	//! コントラストを変更する
	postEffect.colorFilter.x = Easing::Linear(startColorFilterChange.x, endColorFilterChange.x, t);
	postEffect.colorFilter.y = Easing::Linear(startColorFilterChange.y, endColorFilterChange.y, t);
	postEffect.colorFilter.z = Easing::Linear(startColorFilterChange.z, endColorFilterChange.z, t);

	//! 処理を止める
	if (t >= 1.0f)
		colorFilterChange = false;

	return true;
}

//! クロマティックアベレーション(色収差(色ズレ))変更更新処理
bool Camera::UpdateChromaticAberrationChange(float elapsedTime)
{
	//! コントラストを変えないなら、処理を止める
	if (!chromaticAberrationChange)
		return false;


	//! 経過時間を計測
	chromaticAberrationChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = chromaticAberrationChangeCurrentTime / chromaticAberrationChangeTime;

	//! コントラストを変更する
	postEffect.chromaticAberration = Easing::Linear(startChromaticAberrationChange, endChromaticAberrationChange, t);

	//! 処理を止める
	if (t >= 1.0f)
		chromaticAberrationChange = false;

	return true;
}