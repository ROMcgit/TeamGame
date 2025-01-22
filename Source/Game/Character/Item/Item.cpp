#include "Item.h"
#include "ItemManager.h"
#include "Graphics/Graphics.h"

Item::Item(ItemManager* manager)
	: manager(manager)
{
}

// デバッグプリミティブ描画
void Item::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifdef _DEBUG

	debugRenderer->DrawSphere(collisionPos, radius, DirectX::XMFLOAT4(1, 1, 1, 1));

#endif // DEBUG
}

// 破棄
void Item::Destroy()
{
	manager->Remove(this);
}

// 演出の状態更新処理
void Item::UpdateItemState(float elapsedTime, bool updateDirectionTarget, bool rotation, float rotationSpeed)
{
	// 行列更新処理
	UpdateTransform();

	// 当たり判定の位置を設定
	CollisionPosSettings();

	if (updateDirectionTarget)
		// ターゲットを向く更新処理
		UpdateDirectionTarget(elapsedTime, rotation, rotationSpeed);

	// 大きさ変更更新処理
	UpdateScaleChange(elapsedTime);

	// マテリアルの色変更更新処理
	UpdateMaterialColorChange(elapsedTime);

	// 不透明度変更更新処理
	UpdateEffectOpacityChange(elapsedTime);

	//エミッシブの色変更更新処理
	UpdateEmissiveColorChange(elapsedTime);

	//エミッシブの強さ変更更新処理
	UpdateEmissiveStrengthChange(elapsedTime);
}

// 更新行列処理
void Item::UpdateTransform()
{
	DirectX::XMVECTOR Front, Up, Right;

	// 前ベクトルを算出
	Front = DirectX::XMLoadFloat3(&direction);


	// 仮の上ベクトルを算出
	Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	// 右ベクトルを算出
	Right = DirectX::XMVector3Cross(Up, Front);
	Right = DirectX::XMVector3Normalize(Right);


	// 上ベクトルを算出
	Up = DirectX::XMVector3Cross(Front, Right);
	Up = DirectX::XMVector3Normalize(Up);


	// 計算結果を取り出し
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	// 算出した軸ベクトルから行列を作成
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0.0f;
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0.0f;
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0.0f;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

	// 発射方向
	this->direction = front;
}

// 当たり判定の位置を設定
void Item::CollisionPosSettings()
{
	collisionPos = {
		position.x + collisionOffset.x,
		position.y + collisionOffset.y,
		position.z + collisionOffset.z
	};
}

// ターゲットを向く更新処理
void Item::UpdateDirectionTarget(float elapsedTime, bool rotation, float rotationSpeed)
{
	// ターゲット方向ベクトルを計算
	DirectX::XMFLOAT3 targetDir;
	targetDir.x = targetPosition.x - position.x;
	targetDir.y = targetPosition.y - position.y;
	targetDir.z = targetPosition.z - position.z;

	// 現在の方向ベクトルをロード
	DirectX::XMVECTOR currentDirVec = DirectX::XMLoadFloat3(&direction);

	// ターゲット方向ベクトルを正規化してロード
	DirectX::XMVECTOR targetDirVec = DirectX::XMLoadFloat3(&targetDir);
	targetDirVec = DirectX::XMVector3Normalize(targetDirVec);

	// 球面線形補間（Slerp）で滑らかに回転する
	DirectX::XMVECTOR newDirVec = DirectX::XMVectorLerp(currentDirVec, targetDirVec, rotationSpeed * elapsedTime);

	// 結果を正規化して格納
	newDirVec = DirectX::XMVector3Normalize(newDirVec);
	DirectX::XMStoreFloat3(&direction, newDirVec);

	//! 回転するなら
	if (rotation)
	{
		// 角度を回転速度で更新
		angle += rotationSpeed * elapsedTime;

		// 新しい位置を計算 (プレイヤーを中心に円を描く)
		targetPosition.x = position.x + cosf(angle) * distance;
		targetPosition.z = position.z + sinf(angle) * distance;

		// Y座標は固定
		targetPosition.y = position.y;
	}
}

// 大きさ更新処理
bool Item::UpdateScaleChange(float elapsedTime)
{
	//! 大きさを変更しないなら、処理を止める
	if (!scaleChange)
		return false;

	//! 大きさ変更
	scale.x = UpdateScaleAxis(scale.x, scaleChangeSpeed.x, scaleChangeUp.x, toScaleChange.x, elapsedTime);
	scale.y = UpdateScaleAxis(scale.y, scaleChangeSpeed.y, scaleChangeUp.y, toScaleChange.y, elapsedTime);
	scale.z = UpdateScaleAxis(scale.z, scaleChangeSpeed.z, scaleChangeUp.z, toScaleChange.z, elapsedTime);


	// 大きさ変更が終わったら処理を止める
	if (scale.x == toScaleChange.x
		&& scale.y == toScaleChange.y
		&& scale.z == toScaleChange.z)
		scaleChange = false;

	return true;
}

// 単一軸のスケール変更更新処理
float Item::UpdateScaleAxis(float scale, float speed, bool scaleChangeUp, float toScaleChange, float elapsedTime)
{
	// 大きさ変更
	scale += (scaleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目標値を超えた場合、目標値にする
	if ((scaleChangeUp && scale > toScaleChange) || (!scaleChangeUp && scale < toScaleChange))
		scale = toScaleChange;

	// 大きさを返す
	return scale;
}

//! マテリアルの色変更更新処理
bool Item::UpdateMaterialColorChange(float elapsedTime)
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
float Item::UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime)
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
bool Item::UpdateEffectOpacityChange(float elapsedTime)
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

//!エミッシブの色変更更新処理
bool Item::UpdateEmissiveColorChange(float elapsedTime)
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
float Item::UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime)
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

//! エミッシブの強さ変更更新処理
bool Item::UpdateEmissiveStrengthChange(float elapsedTime)
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


