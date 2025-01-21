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

	//! ポストエフェクトのステータスを設定
	void SetPostEffectStatus(float contrast = 1.3f,
		float saturation = 0.7f,
		const DirectX::XMFLOAT3 ColorFilter = { 0.9f, 1.0f, 1.05f },
		float chromaticAberration = 0.01f);

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

	/****************************************************************************************/

		// ポストエフェクトのステータス変更処理
	bool UpdatePostEffectStatusChange(float elapsedTime);

	// カラーフィルターの変更処理
	float UpdateColorComponent(float colorFilter, float speed, float colorFilterChangeUp, float toColorFilter, float elapsedTime);

	// ポストエフェクトのステータスを元に戻す
	bool UpdatepostEffectReset();

private:
	DirectX::XMFLOAT4X4 view;       // ビュー行列
	DirectX::XMFLOAT4X4 projection; // プロジェクション行列

	DirectX::XMFLOAT3 eye;   // カメラの位置
	DirectX::XMFLOAT3 focus; // カメラの注視点

	DirectX::XMFLOAT3 up;    // カメラの上方向
	DirectX::XMFLOAT3 front; // カメラの正面方向
	DirectX::XMFLOAT3 right; // カメラの右方向

private:
	bool postEffectInitialization = false; //! ポストエフェクトのステータスの初期化

public:
	static bool postEffectControll;        //! ゲーム中に数値をいじれるようにする(ImGui用)
	static bool postEffectReset;           //! ポストエフェクトのステータスを元に戻す

	static bool postEffectChange; // ポストエフェクトのステータスを変更するか

	static float toContrastChange;    // ここまでコンストラクトを変える
	static float contrastChangeSpeed; // コンストラクトの値を変える速度
	static bool  contrastUp;          // コンストラクトの値を上昇させるか

	static float toSaturationChange;    // ここまでサチュレーション(色の彩度)を変える
	static float saturationChangeSpeed; // サチュレーションの値を変える速度
	static bool  saturationUp;          // サチュレーション値を上昇させるか

	static DirectX::XMFLOAT3 toColorFilterChange;    // ここまでカラーフィルター(色フィルター)の値を変える
	static DirectX::XMFLOAT3 colorFilterChangeSpeed; // カラーフィルターの値を変える速度
	struct ColorFilterUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};
	static ColorFilterUp colorFilterUp;   // カラーフィルターの値を上昇させるか

	static float toChromaticAberrationChange;    // ここまでクロマティックアベレーション(色収差(色ズレ))の値を変える
	static float chromaticAberrationChangeSpeed; // クロマティックアベレーションの値を変える速度
	static bool  chromaticAberrationUp;          // クロマティックアベレーションの値を上昇させるか
};