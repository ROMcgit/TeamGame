#pragma once

#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>

// カメラ
class Camera
{
public:
	//! ポストエフェクトを生成
	void CreatePostEffect();

	//! ポストエフェクトのステータスの設定(一回だけ)フラグを初期化
	void SetPostEffectStatusOnceUnlock() { this->setPostEffectStatusOnce = false; }

	//! ポストエフェクトのステータスの設定(一回だけ)
	void SetPostEffectStatusOnce();

	//! ポストエフェクトのステータスを設定
	void SetPostEffectStatus();
public:
	//! ポストエフェクト関連
	struct CBPostEffectParam
	{
		float contrast;            // コンストラクト(数値が大きいほど、明るい部分はより明るく、暗い部分はより暗くなる)
		float saturation;          // サチュレーション(色の彩度)
		float chromaticAberration; // クロマティックアベレーション(色収差(色ズレ))
		float hueShift;            // 色相シフト（-1.0 ～ 1.0）

		DirectX::XMFLOAT3 colorFilter; // カラーフィルター(色フィルター)
		float dummy1;                  // 4個区切り用

		float vignetteIntensity; // ビネットの強度
		float vignetteOpacity;   // ビネットの不透明度
		float blurStrength;      // ブラーの強度
		float dummy2;            // 4個区切り用

		float bloomThreshold;     // グレアの閾値
		float bloomIntensity;     // グレアの強度
		DirectX::XMFLOAT2 dummy3; // 4個区切り用

		float focusDistance;   // ピントを合わせる距離
		float focusRange;      // ピントが合う範囲
		float dofBlurStrength; // 被写界深度のブラー強度
		float dummy4;          // 4個区切り用

		DirectX::XMFLOAT2 shakeOffset; // シェイクのオフセット
		float shakeStrength;           // シェイクの強度
		float shakeTime;               // 外部から渡す時間
		float shakeSpeed;              // シェイクの速度
		DirectX::XMFLOAT3 dummy5;      // 4個区切り用
	};
	static CBPostEffectParam postEffect;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CBPostEffect;

public:
	Camera();
	~Camera() {}

public:
	// 唯一のインスタンス取得
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}

	// 更新処理
	void Update(float elapsedTime);

	// デバッグGUI描画
	void DrawDebugGUI();

	/****************************************************************************************/
		/*! セッター */

		// 指定方向を向く
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
		const DirectX::XMFLOAT3& up);

	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// カメラシェイク(シェーダー)の設定
	void SetCameraShakeShader(float cameraShakeTime = 1.0f, float cameraShakeStrength = 0.2f, DirectX::XMFLOAT2 cameraShakeOffset = { 0, 0.05f }, float cameraShakeSpeed = 50.0f)
	{
		if (!cameraShakeShader)
		{
			cameraShakeShader = true;                // カメラシェイク(シェーダー)をする
			cameraShakeShaderTime = cameraShakeTime;     // カメラシェイク(シェーダー)の時間
			postEffect.shakeStrength = cameraShakeStrength; // カメラシェイク(シェーダー)の強さ
			postEffect.shakeOffset = cameraShakeOffset;   // カメラシェイク(シェーダー)のずらす位置
			postEffect.shakeSpeed = cameraShakeSpeed;    // カメラシェイク(シェーダー)の速さ
		}
	}

	/****************************************************************************************/
		/*! ゲッター */

#if 1
	// ビュー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// カメラの位置取得
	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	// 注視点取得
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }
	// カメラの上方向取得
	const DirectX::XMFLOAT3& GetUp()    const { return up; }
	// カメラの正面方向取得
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	// カメラの右方向取得
	const DirectX::XMFLOAT3& GetRight() const { return right; }

#endif

	/****************************************************************************************/

		// ポストエフェクトのリセット
	bool PostEffectStatusReset();

	// ポストエフェクトのステータス変更処理
	void UpdatePostEffectStatusChange(float elapsedTime);

	// コントラスト変更更新処理
	bool UpdateContrastChange(float elapsedTime);

	// サチュレーション(色の彩度)変更更新処理
	bool UpdateSaturationChange(float elapsedTime);

	// カラーフィルター変更更新処理
	bool UpdateColorFilterChange(float elapsedTime);

	// クロマティックアベレーション(色収差(色ズレ))変更更新処理
	bool UpdateChromaticAberrationChange(float elapsedTime);

	// ブラーの強度変更更新処理
	bool UpdateBlurStrengthChange(float elapsedTime);

	// カメラシェイク更新処理
	bool UpdateCameraShakeShader(float elapsedTime);

	bool UpdateVignetteIntensityChange(float elapsedTime);

private:
	DirectX::XMFLOAT4X4 view;       // ビュー行列
	DirectX::XMFLOAT4X4 projection; // プロジェクション行列

	DirectX::XMFLOAT3 eye;   // カメラの位置
	DirectX::XMFLOAT3 focus; // カメラの注視点

	DirectX::XMFLOAT3 up;    // カメラの上方向
	DirectX::XMFLOAT3 front; // カメラの正面方向
	DirectX::XMFLOAT3 right; // カメラの右方向

private:
	bool setPostEffectStatusOnce = false; //! ポストエフェクトのステータスの設定(一回だけ)をしたか

public:
	/*! リセット */
#if 1
	static bool  postEffectReset;              //! ポストエフェクトのリセット
	static float contrastReset;                // コントラストのリセット時の数値
	static float saturationReset;              // サチュレーションのリセットの数値
	static DirectX::XMFLOAT3 colorFilterReset; // カラーフィルターのリセットの数値
	static float chromaticAberrationReset;     // クロマティックアベレーションのリセットの数値
	static float hueShiftRest;                 // 色相シフトのリセット数値
	static float blurStrengthReset;            // ブラーの強度のリセットの数値
	static float bloomThresholdReset;          // グレアの閾値のリセットの数値
	static float bloomIntensityReset;          // グレアの強度のリセットの数値
	static float focusDistanceReset;           // ピントを合わせる距離のリセットの数値
	static float focusRangeReset;              // ピントが合う範囲のリセットの数値
	static float dofBlurStrengthRest;          // 被写界深度のブラー強度のリセットの数値
#endif

	static bool  contrastChange;            // コントラストを変えるか
	static float startContrastChange;       // コントラストの変更の開始の値
	static float endContrastChange;         // ここまでコントラストを変える
	static float contrastChangeTime;        // コントラストの値を変える時間
	static float contrastChangeElapsedTime; // コントラスト変更の経過時間

	static bool  saturationChange;            // サチュレーションを変えるか
	static float startSaturationChange;       // サチュレーションの変更の開始の値
	static float endSaturationChange;         // ここまでサチュレーション(色の彩度)を変える
	static float saturationChangeTime;        // サチュレーションの値を変える時間
	static float saturationChangeElapsedTime; // サチュレーション変更の経過時間

	static bool colorFilterChange;                   // カラーフィルターを変えるか
	static DirectX::XMFLOAT3 startColorFilterChange; // カラーフィルターの変更の開始の値
	static DirectX::XMFLOAT3 endColorFilterChange;   // ここまでカラーフィルター(色フィルター)の値を変える
	static float colorFilterChangeTime;              // カラーフィルターの値を変える時間
	static float colorFilterChangeElapsedTime;       // カラーフィルター変更の経過時間

	static bool  chromaticAberrationChange;            // クロマティックアベレーションを変えるか
	static float startChromaticAberrationChange;       // クロマティックアベレーションの変更の開始の値
	static float endChromaticAberrationChange;         // ここまでクロマティックアベレーション(色収差(色ズレ))の値を変える
	static float chromaticAberrationChangeTime;        // クロマティックアベレーションの値を変える時間
	static float chromaticAberrationChangeElapsedTime; // クロマティックアベレーション変更の経過時間

	static bool  blurStrengthChange;            // ブラーの強度を変えるか
	static float startBlurStrengthChange;       // ブラーの強度の変更の開始の値
	static float endBlurStrengthChange;         // ここまでブラーの強度を変える
	static float blurStrengthChangeTime;        // ブラーの強度を変える時間
	static float blurStrengthChangeElapsedTime; // ブラーの強度変更の経過時間

	static bool  cameraShakeShader;     // カメラシェイク(シェーダー)
	static float cameraShakeShaderTime; // カメラシェイク(シェーダー)する時間

	static bool vignetteIntensityChange;
	static float startVignetteIntensityChange;
	static float endVignetteIntensityChange;
	static float vignetteIntensityChangeTime;
	static float vignetteIntensityChangeElapsedTime;

	//static bool vignetteOpacityChange;

};