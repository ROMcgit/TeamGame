#pragma once

#include "Graphics/Shader.h"
#include "Other/Collision.h"

// ステージ
class Stage
{
public:
	Stage() {}
	virtual ~Stage() {};

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// レイキャスト
	virtual bool RayCast(const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	// デバッグ用GUI描画
	virtual void DrawDebugGUI() {};

	/*****************************************************************************************************************************/
		/*! セッター */

		// マテリアルの色の設定
	void SetMaterialColor(const DirectX::XMFLOAT3 materialColor) { this->materialColor = materialColor; }

	// マテリアルの色変更の設定
	void SetMaterialColorChange(DirectX::XMFLOAT3 toMaterialColorChange, DirectX::XMFLOAT3 materialColorChangeSpeed)
	{
		if (!this->materialColorChange)
		{
			this->materialColorChange = true;                     // マテリアルの色を変える
			this->toMaterialColorChange = toMaterialColorChange;    // ここまでマテリアルの色を変える
			this->materialColorChangeSpeed = materialColorChangeSpeed; // マテリアルの色を変える速さ

			// マテリアルカラーXが目指す値より小さい時
			if (materialColor.x < toMaterialColorChange.x)
				materialColorChangeUp.x = true;
			else
				materialColorChangeUp.x = false;

			// マテリアルカラーYが目指す値より小さい時
			if (materialColor.y < toMaterialColorChange.y)
				materialColorChangeUp.y = true;
			else
				materialColorChangeUp.y = false;

			// マテリアルカラーZが目指す値より小さい時
			if (materialColor.z < toMaterialColorChange.z)
				materialColorChangeUp.z = true;
			else
				materialColorChangeUp.z = false;
		}
	}

	// 不透明度を設定
	void SetOpacity(float opacity) { this->opacity = opacity; }

	// 不透明度変更処理の設定
	void SetOpacityChange(float toOpacityChange, float opacityChangeSpeed)
	{
		if (!this->opacityChange)
		{
			this->opacityChange = true;               // 不透明度を変える
			this->toOpacityChange = toOpacityChange;    // ここまで不透明度を変える
			this->opacityChangeSpeed = opacityChangeSpeed; // 不透明度を変える速さ

			// 不透明度が目指す値より小さい時
			if (opacity < toOpacityChange)
				opacityChangeUp = true;
			else
				opacityChangeUp = false;
		}
	}

	//-----------------------------------------------------------------------------//

		// エミッシブの色の設定
	void SetEmissiveColor(const DirectX::XMFLOAT3 emissiveColor) { this->emissiveColor = emissiveColor; }

	// エミッシブの色変更の設定
	void SetEmissiveColorChange(DirectX::XMFLOAT3 toEmissiveColorChange, DirectX::XMFLOAT3 emissiveColorChangeSpeed)
	{
		if (!this->emissiveColorChange)
		{
			this->emissiveColorChange = true;                     //エミッシブの色を変える
			this->toEmissiveColorChange = toEmissiveColorChange;    // ここまでエミシッブの色を変える
			this->emissiveColorChangeSpeed = emissiveColorChangeSpeed; //エミッシブの色を変える速さ

			//エミッシブカラーXが目指す値より小さい時
			if (emissiveColor.x < toEmissiveColorChange.x)
				emissiveColorChangeUp.x = true;
			else
				emissiveColorChangeUp.x = false;

			//エミッシブカラーYが目指す値より小さい時
			if (emissiveColor.y < toEmissiveColorChange.y)
				emissiveColorChangeUp.y = true;
			else
				emissiveColorChangeUp.y = false;

			//エミッシブカラーZが目指す値より小さい時
			if (emissiveColor.z < toEmissiveColorChange.z)
				emissiveColorChangeUp.z = true;
			else
				emissiveColorChangeUp.z = false;
		}
	}

	// エミッシブの強さの設定
	void SetEmissiveStrength(float emissiveStrength) { this->emissiveStrength = emissiveStrength; }

	// エミッシブの強さ変更の設定(設定の時に色を変えれる)
	void SetEmissiveStrengthChange(DirectX::XMFLOAT3 emissiveColor, float toEmissiveStrengthChange, float emissiveStrengthChangeSpeed)
	{
		if (!this->emissiveStrengthChange)
		{
			this->emissiveColor = emissiveColor;               //エミッシブの色
			this->emissiveStrengthChange = true;                        //エミッシブの強さを変える
			this->toEmissiveStrengthChange = toEmissiveStrengthChange;    // ここまでエミッシブの強さを変える
			this->emissiveStrengthChangeSpeed = emissiveStrengthChangeSpeed; // エミッシブの強さを変える速さ

			//!エミッシブの強さが目指す値より小さい時
			if (emissiveStrength < toEmissiveStrengthChange)
				emissiveStrengthChangeUp = true;
			else
				emissiveStrengthChangeUp = false;
		}
	}

	//----------------------------------------------------------------------------------------------------//

		// 状態更新処理
	void UpdateStageState(float elapsedTime);

	// 行列更新処理
	void UpdateTransform();

	// マテリアルの色変更更新処理
	bool UpdateMaterialColorChange(float elapsedTime);

	// 単一軸のマテリアルの色変更更新処理
	float UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime);

	// 不透明度変更更新処理
	bool UpdateEffectOpacityChange(float elapsedTime);

	// エミッシブの色変更更新処理
	bool UpdateEmissiveColorChange(float elapsedTime);

	// 単一軸のエミシッブの色変更更新処理
	float UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime);

	// エミッシブの強さ変更更新処理
	bool UpdateEmissiveStrengthChange(float elapsedTime);

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT4X4 oldTransform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };

	/***************************************************************************************/
		/*! マテリアル */
	DirectX::XMFLOAT3 materialColor = { 1,1,1 }; // マテリアルの色
	bool materialColorChange = false;            // マテリアルの色を変更するか
	//マテリアルの色を増やすか
	struct MaterialColorChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}materialColorChangeUp;  // マテリアルの色を増やすか

	DirectX::XMFLOAT3 toMaterialColorChange = { 0, 0, 0 }; // ここまでマテリアルの色を変える
	DirectX::XMFLOAT3 materialColorChangeSpeed = { 0, 0, 0 }; // マテリアルの色を変える速さ

	//! 不透明度
	float opacity = 1.0f;    // 不透明度
	bool  opacityChange = false;   // 不透明度を変更するか
	bool  opacityChangeUp = false;   // 不透明度を増やすか
	float toOpacityChange = 0.0f;    // ここまで不透明度を変える 
	float opacityChangeSpeed = 0.0f; // 不透明度を変える速度

	/***************************************************************************************/
		/*!エミッシブ */
	DirectX::XMFLOAT3 emissiveColor = { 0.0f, 0.0f, 0.0f }; //エミッシブカラー
	bool emissiveColorChange = false; //エミッシブの色を変えるか
	//エミッシブの色を増やすか
	struct EmissiveColorChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}emissiveColorChangeUp;  //エミッシブの色を増やすか

	DirectX::XMFLOAT3 toEmissiveColorChange = { 0, 0, 0 }; // ここまでエミシッブの色を変える
	DirectX::XMFLOAT3 emissiveColorChangeSpeed = { 0, 0, 0 }; //エミッシブの色を変える速さ

	//!エミッシブの強さ
	float emissiveStrength = 0.0f;  //エミッシブの強さ
	bool  emissiveStrengthChange = false; //エミッシブの強さを変えるか
	bool  emissiveStrengthChangeUp = false; //エミッシブの強さを増やすか

	float toEmissiveStrengthChange = 0.0f;  //　ここまでエミシッブの強さを変える
	float emissiveStrengthChangeSpeed = 0.0f;  //　エミシッブの強さを変える速さ
};