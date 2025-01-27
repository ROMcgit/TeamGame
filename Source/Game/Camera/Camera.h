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

	//! ポストエフェクトのステータスの設定(一回だけ)
	void SetPostEffectStatusOnce(float contrast = 1.0f,
		float saturation = 0.8f,
		const DirectX::XMFLOAT3 colorFilter = { 1.2f, 1.3f, 1.35f },
		float chromaticAberration = 0);

	//! ポストエフェクトのステータスを設定
	void SetPostEffectStatus(float contrast = 1.0f,
		float saturation = 0.8f,
		const DirectX::XMFLOAT3 colorFilter = { 1.2f, 1.3f, 1.35f },
		float chromaticAberration = 0);

public:
	//! ポストエフェクト関連
	struct CBPostEffectParam
	{
		float contrast;            // コンストラクト(数値が大きいほど、明るい部分はより明るく、暗い部分はより暗くなる)
		float saturation;          // サチュレーション(色の彩度)
		float chromaticAberration; // クロマティックアベレーション(色収差(色ズレ))
		float dummy;  // 4個区切り用

		DirectX::XMFLOAT3 colorFilter; // カラーフィルター(色フィルター)
		float dummy2; // 4個区切り用
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
	static bool postEffectControll;        //! ゲーム中に数値をいじれるようにする(ImGui用)
	static bool postEffectReset;           //! ポストエフェクトのステータスを元に戻す

	static bool  contrastChange;            // コントラストを変えるか
	static float startContrastChange;       // コントラストの変更の開始の値
	static float endContrastChange;         // ここまでコントラストを変える
	static float contrastChangeTime;        // コントラストの値を変える時間
	static float contrastChangeCurrentTime; // コントラスト変更の経過時間

	static bool  saturationChange;            // サチュレーションを変えるか
	static float startSaturationChange;       // サチュレーションの変更の開始の値
	static float endSaturationChange;         // ここまでサチュレーション(色の彩度)を変える
	static float saturationChangeTime;        // サチュレーションの値を変える時間
	static float saturationChangeCurrentTime; // サチュレーション変更の経過時間

	static bool colorFilterChange;                   // カラーフィルターを変えるか
	static DirectX::XMFLOAT3 startColorFilterChange; // カラーフィルターの変更の開始の値
	static DirectX::XMFLOAT3 endColorFilterChange;   // ここまでカラーフィルター(色フィルター)の値を変える
	static float colorFilterChangeTime;              // カラーフィルターの値を変える時間
	static float colorFilterChangeCurrentTime;       // カラーフィルター変更の経過時間

	static bool  chromaticAberrationChange;            // クロマティックアベレーションを変えるか
	static float startChromaticAberrationChange;       // クロマティックアベレーションの変更の開始の値
	static float endChromaticAberrationChange;         // ここまでクロマティックアベレーション(色収差(色ズレ))の値を変える
	static float chromaticAberrationChangeTime;        // クロマティックアベレーションの値を変える時間
	static float chromaticAberrationChangeCurrentTime; // クロマティックアベレーション変更の経過時間
};