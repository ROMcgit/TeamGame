#include "Stage.h"

//! 状態更新処理
void Stage::UpdateStageState(float elapsedTime)
{
	// 行列更新処理
	UpdateTransform();

	// 不透明度変更更新処理
	UpdateEffectOpacityChange(elapsedTime);

	// エミッシブの色変更更新処理
	UpdateEmissiveColorChange(elapsedTime);

	// エミッシブの強さ変更更新処理
	UpdateEmissiveStrengthChange(elapsedTime);
}

// 行列更新処理
void Stage::UpdateTransform()
{
	// 以前の変換行列を保存
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

// マテリアルの色変更更新処理
bool Stage::UpdateMaterialColorChange(float elapsedTime)
{
	//! マテリアルの色を変えないなら、処理を止める
	if (!materialColorChange)
		return false;


	//! マテリアルの色を変更
	materialColor.x = UpdateMaterialColorAxis(materialColor.x, materialColorChangeSpeed.x, materialColorChangeUp.x, toMaterialColorChange.x, elapsedTime);
	materialColor.y = UpdateMaterialColorAxis(materialColor.y, materialColorChangeSpeed.y, materialColorChangeUp.y, toMaterialColorChange.y, elapsedTime);
	materialColor.z = UpdateMaterialColorAxis(materialColor.z, materialColorChangeSpeed.z, materialColorChangeUp.z, toMaterialColorChange.z, elapsedTime);

	//! マテリアルの色変更が終わったら、処理を止める
	if (materialColor.x == toMaterialColorChange.x
		&& materialColor.y == toMaterialColorChange.y
		&& materialColor.z == toMaterialColorChange.z)
		materialColorChange = false;

	return true;
}

// 単一軸のマテリアルの色変更更新処理
float Stage::UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime)
{
	// マテリアルの色変更
	materialColor += (materialColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((materialColorChangeUp && materialColor > toMaterialColorChange) || (!materialColorChangeUp && materialColor < toMaterialColorChange))
	{
		materialColor = toMaterialColorChange;
	}

	// マテリアルの色を返す
	return materialColor;
}

//! 不透明度変更更新処理
bool Stage::UpdateEffectOpacityChange(float elapsedTime)
{
	//! 不透明度を変更しないなら、処理を止める
	if (!opacityChange)
		return false;


	//! 不透明度を変更
	opacity += (opacityChangeUp ? 1 : -1) * opacityChangeSpeed * elapsedTime;

	//! 目指す値を超えた場合、目標値にする
	if ((opacityChangeUp && opacity > toOpacityChange) || (!opacityChangeUp && opacity < toOpacityChange))
		opacity = toOpacityChange;

	//! 不透明度の変更が終わったら
	if (opacity == toOpacityChange)
		opacityChange = false;

	return true;
}

//! エミッシブの色変更更新処理
bool Stage::UpdateEmissiveColorChange(float elapsedTime)
{
	//!エミッシブの色を変えないなら、処理を止める
	if (!emissiveColorChange)
		return false;


	//!エミッシブの色を変更
	emissiveColor.x = UpdateEmissiveColorAxis(emissiveColor.x, emissiveColorChangeSpeed.x, emissiveColorChangeUp.x, toEmissiveColorChange.x, elapsedTime);
	emissiveColor.y = UpdateEmissiveColorAxis(emissiveColor.y, emissiveColorChangeSpeed.y, emissiveColorChangeUp.y, toEmissiveColorChange.y, elapsedTime);
	emissiveColor.z = UpdateEmissiveColorAxis(emissiveColor.z, emissiveColorChangeSpeed.z, emissiveColorChangeUp.z, toEmissiveColorChange.z, elapsedTime);

	//!エミッシブの色変更が終わったら、処理を止める
	if (emissiveColor.x == toEmissiveColorChange.x
		&& emissiveColor.y == toEmissiveColorChange.y
		&& emissiveColor.z == toEmissiveColorChange.z)
		emissiveColorChange = false;

	return true;
}

// 単一軸のエミシッブの色変更更新処理
float Stage::UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime)
{
	//エミッシブの色変更
	emissiveColor += (emissiveColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((emissiveColorChangeUp && emissiveColor > toEmissiveColorChange) || (!emissiveColorChangeUp && emissiveColor < toEmissiveColorChange))
	{
		emissiveColor = toEmissiveColorChange;
	}

	//エミッシブの色を返す
	return emissiveColor;
}

// エミッシブの強さ変更更新処理
bool Stage::UpdateEmissiveStrengthChange(float elapsedTime)
{
	//!エミッシブの強さを変えないなら、処理を止める
	if (!emissiveStrengthChange)
		return false;

	//!エミッシブの強さを変更
	emissiveStrength += (emissiveStrengthChangeUp ? 1 : -1) * emissiveStrengthChangeSpeed * elapsedTime;

	//! 目指す値を超えた場合、目標値にする
	if ((emissiveStrengthChangeUp && emissiveStrength > toEmissiveStrengthChange) || (!emissiveStrengthChangeUp && emissiveStrength < toEmissiveStrengthChange))
		emissiveStrength = toEmissiveStrengthChange;

	//!エミッシブの強さの変更が終わったら、処理を止める
	if (emissiveStrength == toEmissiveStrengthChange)
		emissiveStrengthChange = false;

	return true;
}
