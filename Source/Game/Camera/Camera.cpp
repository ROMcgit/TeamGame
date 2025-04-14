#include "Camera.h"
#include "Graphics/ShaderSetting.h"
#include "Graphics/Graphics.h"
#include "Game/Scene/Scene.h"
#include "Other/Easing.h"
#include <imgui.h>

//! ポストエフェクト
Camera::CBPostEffectParam Camera::postEffect = {};
Microsoft::WRL::ComPtr<ID3D11Buffer> Camera::CBPostEffect;

//----------------------------------------------------------//

//! ポストエフェクトの数値をリセットするか
bool Camera::postEffectReset = false;
//! コントラストのリセット時の数値
float Camera::contrastReset;
//! サチュレーションのリセットの数値
float Camera::saturationReset;
//! カラーフィルターのリセットの数値
DirectX::XMFLOAT3 Camera::colorFilterReset;
//! クロマティックアベレーションのリセットの数値
float Camera::chromaticAberrationReset;
//! 色相シフトのリセット数値
float Camera::hueShiftRest;
//! ブラーの強度のリセットの数値
float Camera::blurStrengthReset;
//! グレアの閾値のリセットの数値
float Camera::bloomThresholdReset;
//! グレアの強度のリセットの数値
float Camera::bloomIntensityReset;
//! ピントを合わせる距離のリセットの数値
float Camera::focusDistanceReset;
//! ピントが合う範囲のリセットの数値
float Camera::focusRangeReset;
//! 被写界深度のブラー強度のリセットの数値
float Camera::dofBlurStrengthRest;

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
float Camera::contrastChangeElapsedTime = 0.0f;

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
float Camera::saturationChangeElapsedTime = 0.0f;

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
float Camera::colorFilterChangeElapsedTime = 0.0f;

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
float Camera::chromaticAberrationChangeElapsedTime = 0.0f;

//----------------------------------------------------------//

//! ブラーの強度を変更するか
bool Camera::blurStrengthChange = false;
//! ブラーの強度の変更開始の値
float Camera::startBlurStrengthChange = 0.0f;
//! ここまでブラーの強度を変える
float Camera::endBlurStrengthChange = 0.0f;
//! ブラーの強度を変える時間
float Camera::blurStrengthChangeTime = 0.0f;
//! ブラーの強度変更の経過時間
float Camera::blurStrengthChangeElapsedTime = 0.0f;

//----------------------------------------------------------//

//! カメラシェイク(シェーダー)をするか
bool Camera::cameraShakeShader = false;
//! カメラシェイク(シェーダー)をする時間
float Camera::cameraShakeShaderTime = 0.0f;

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
void Camera::SetPostEffectStatusOnce()
{
	// パラメータ初期化
	if (!setPostEffectStatusOnce)
	{
		//! 変更処理を解除
		contrastChange = false; // コントラストの変更
		saturationChange = false; // サチュレーションの変更
		colorFilterChange = false; // カラーフィルターの変更
		chromaticAberrationChange = false; // クロマティックアベレーションの変更
		blurStrengthChange = false; // ブラーの強度の変更

		//! ポストエフェクト
		postEffect.contrast = contrastReset = Scene::contrastStatic;            // コントラスト
		postEffect.saturation = saturationReset = Scene::saturationStatic;          // サチュレーション
		postEffect.colorFilter = colorFilterReset = Scene::colorFilterStatic;         // カラーフィルター
		postEffect.chromaticAberration = chromaticAberrationReset = Scene::chromaticAberrationStatic; // クロマティックアベレーション
		postEffect.hueShift = hueShiftRest = Scene::hueShiftStatic;            // 色相シフト 

		postEffect.vignetteIntensity = Scene::vignetteIntensityStatic;                    // ビネットの強度
		postEffect.vignetteOpacity = Scene::vignetteOpacityStatic;                      // ビネットの不透明度
		postEffect.blurStrength = blurStrengthReset = Scene::blurStrengthStatic;   // ブラーの強度
		postEffect.bloomThreshold = bloomThresholdReset = Scene::bloomThresholdStatic; // グレアの閾値
		postEffect.bloomIntensity = bloomIntensityReset = Scene::bloomIntensityStatic; // グレアの強度

		postEffect.focusDistance = focusDistanceReset = Scene::focusDistanceStatic;   // ピントを合わせる距離
		postEffect.focusRange = focusRangeReset = Scene::focusRangeStatic;      // ピントが合う範囲
		postEffect.dofBlurStrength = dofBlurStrengthRest = Scene::dofBlurStrengthStatic; // 被写界深度のブラー強度

		postEffect.shakeOffset = { 0.0f, 0.0f }; // カメラシェイク(シェーダー)のずらす位置
		postEffect.shakeStrength = 0.0f;           // カメラシェイク(シェーダー)の強さ
		postEffect.shakeTime = 0.0f;           // カメラシェイク(シェーダー)の時間
		postEffect.shakeSpeed = 0.0f;           // カメラシェイク(シェーダー)の速さ

		setPostEffectStatusOnce = true; // ポストエフェクトのステータスの設定(一回だけ)をした
	}
}

// ポストエフェクトのステータスを設定
void Camera::SetPostEffectStatus()
{
	//! パラメータの設定
	if (!postEffectReset)
	{
		postEffect.contrast = Scene::contrastStatic;            // コントラスト
		postEffect.saturation = Scene::saturationStatic;          // サチュレーション
		postEffect.colorFilter = Scene::colorFilterStatic;         // カラーフィルター
		postEffect.chromaticAberration = Scene::chromaticAberrationStatic; // クロマティックアベレーション
		postEffect.hueShift = Scene::hueShiftStatic;            // 色相シフト

		postEffect.vignetteIntensity = Scene::vignetteIntensityStatic; // ビネットの強度
		postEffect.vignetteOpacity = Scene::vignetteOpacityStatic;   // ビネットの不透明度
		postEffect.blurStrength = Scene::blurStrengthStatic;      // ブラーの強度
		postEffect.bloomThreshold = Scene::bloomThresholdStatic;    // グレアの閾値
		postEffect.bloomIntensity = Scene::bloomIntensityStatic;    // グレアの強度

		postEffect.focusDistance = Scene::focusDistanceStatic;   // ピントを合わせる距離
		postEffect.focusRange = Scene::focusRangeStatic;      // ピントが合う範囲
		postEffect.dofBlurStrength = Scene::dofBlurStrengthStatic; // 被写界深度のブラー強度
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

// ポストエフェクトのリセット
bool Camera::PostEffectStatusReset()
{
	//! ポストエフェクトのリセットをしないなら、処理を止める
	if (!postEffectReset)
		return false;

	postEffect.contrast = contrastReset;            // コントラスト
	postEffect.saturation = saturationReset;          // サチュレーション
	postEffect.colorFilter = colorFilterReset;         // カラーフィルター
	postEffect.chromaticAberration = chromaticAberrationReset; // クロマティックアベレーション
	postEffect.hueShift = hueShiftRest;             // 色相シフト

	postEffect.vignetteIntensity = 0.0f;                // ビネットの強度
	postEffect.vignetteOpacity = 0.0f;                // ビネットの不透明度
	postEffect.blurStrength = blurStrengthReset;   // ブラーの強度
	postEffect.bloomThreshold = bloomThresholdReset; // グレアの閾値
	postEffect.bloomIntensity = bloomIntensityReset; // グレアの強度

	postEffect.focusDistance = focusDistanceReset;  // ピントを合わせる距離
	postEffect.focusRange = focusRangeReset;     // ピントが合う範囲
	postEffect.dofBlurStrength = dofBlurStrengthRest; // 被写界深度のブラー強度

	//! ポストエフェクトのリセットを解除
	postEffectReset = false;

	return true;
}

//! ポストエフェクトのステータス変更処理
void Camera::UpdatePostEffectStatusChange(float elapsedTime)
{
	//! ポストエフェクトのリセット
	PostEffectStatusReset();

	//! コントラスト変更更新処理
	UpdateContrastChange(elapsedTime);

	//! サチュレーション変更更新処理
	UpdateSaturationChange(elapsedTime);

	//! カラーフィルター変更更新処理
	UpdateColorFilterChange(elapsedTime);

	//! クロマティックアベレーション変更更新処理
	UpdateChromaticAberrationChange(elapsedTime);

	//! ブラーの強度変更更新処理
	UpdateBlurStrengthChange(elapsedTime);

	//! カメラシェイク(シェーダー)更新処理
	UpdateCameraShakeShader(elapsedTime);
}

//! コントラスト変更更新処理
bool Camera::UpdateContrastChange(float elapsedTime)
{
	//! コントラストを変えないなら、処理を止める
	if (!contrastChange)
		return false;


	//! 経過時間を計測
	contrastChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = contrastChangeElapsedTime / contrastChangeTime;

	//! コントラストを変更する
	postEffect.contrast = Easing::Linear(startContrastChange, endContrastChange, t);

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		postEffect.contrast = endContrastChange;
		contrastChange = false;
	}

	return true;
}

//! サチュレーション(色の彩度)変更更新処理
bool Camera::UpdateSaturationChange(float elapsedTime)
{
	//! サチュレーションを変えないなら、処理を止める
	if (!saturationChange)
		return false;


	//! 経過時間を計測
	saturationChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = saturationChangeElapsedTime / saturationChangeTime;

	//! サチュレーションを変更する
	postEffect.saturation = Easing::Linear(startSaturationChange, endSaturationChange, t);

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		postEffect.saturation = endSaturationChange;
		saturationChange = false;
	}

	return true;
}

//! カラーフィルター変更更新処理
bool Camera::UpdateColorFilterChange(float elapsedTime)
{
	//! カラーフィルターを変えないなら、処理を止める
	if (!colorFilterChange)
		return false;


	//! 経過時間を計測
	colorFilterChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = colorFilterChangeElapsedTime / colorFilterChangeTime;

	//! カラーフィルターを変更する
	postEffect.colorFilter.x = Easing::Linear(startColorFilterChange.x, endColorFilterChange.x, t);
	postEffect.colorFilter.y = Easing::Linear(startColorFilterChange.y, endColorFilterChange.y, t);
	postEffect.colorFilter.z = Easing::Linear(startColorFilterChange.z, endColorFilterChange.z, t);

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		postEffect.colorFilter = endColorFilterChange;
		colorFilterChange = false;
	}

	return true;
}

//! クロマティックアベレーション(色収差(色ズレ))変更更新処理
bool Camera::UpdateChromaticAberrationChange(float elapsedTime)
{
	//! コントラストを変えないなら、処理を止める
	if (!chromaticAberrationChange)
		return false;


	//! 経過時間を計測
	chromaticAberrationChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = chromaticAberrationChangeElapsedTime / chromaticAberrationChangeTime;

	//! クロマティックアベレーションを変更する
	postEffect.chromaticAberration = Easing::Linear(startChromaticAberrationChange, endChromaticAberrationChange, t);

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		postEffect.chromaticAberration = endChromaticAberrationChange;
		chromaticAberrationChange = false;
	}

	return true;
}

//! ブラーの強度変更更新処理
bool Camera::UpdateBlurStrengthChange(float elapsedTime)
{
	//! ブラーの強度を変えないなら、処理を止める
	if (!blurStrengthChange)
		return false;


	//! 経過時間を計測
	blurStrengthChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = blurStrengthChangeElapsedTime / blurStrengthChangeTime;

	//! ブラーの強度を変更する
	postEffect.blurStrength = Easing::Linear(startBlurStrengthChange, endBlurStrengthChange, t);

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		postEffect.blurStrength = endBlurStrengthChange;
		chromaticAberrationChange = false;
	}

	return true;
}

// カメラシェイク(シェーダー)更新処理
bool Camera::UpdateCameraShakeShader(float elapsedTime)
{
	//! カメラシェイク(シェーダー)をしないなら、処理を止める
	if (!cameraShakeShader)
		return false;

	//! カメラシェイク(シェーダー)の時間を計測
	postEffect.shakeTime += elapsedTime;

	//! 指定した時間を超えたら、処理を止める
	if (postEffect.shakeTime > cameraShakeShaderTime)
	{
		postEffect.shakeTime = 0.0f;  // カメラシェイクの計測時間を0にする
		postEffect.shakeStrength = 0.0f;  // カメラシェイクの強さを0にする
		cameraShakeShader = false; // カメラシェイクを解除
	}

	return true;
}
