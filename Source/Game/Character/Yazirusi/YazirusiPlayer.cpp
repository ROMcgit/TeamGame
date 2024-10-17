#include "Game/Character/Yazirusi/YazirusiPlayer.h"
#include "Game/Character/Yazirusi/YazirusiManager.h"
#include "Game/Character/Player.h"
#include "Game/Character/Item/ImportantItem.h"
#include "Game/Character/Item/ImportantItemManager.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/Enemy.h"

// コンストラクタ
YazirusiPlayer::YazirusiPlayer(YazirusiManager* manager) : Yazirusi(manager)
{
	model = new Model("./Data/Model/Monkey/0.mdl/Yazirusi.mdl");

	// 表示サイズを調整
	scale.x = scale.y = scale.z = 0.02f;

	collisionOffset = { 0, 10.0f, 0 };

	radius = 0.0f;
}

// デストラクタ
YazirusiPlayer::~YazirusiPlayer()
{
	delete model;
}

// 更新処理
void YazirusiPlayer::Update(float elapsedTime)
{
	// 当たり判定の位置設定
	CollisionPosSettings();

	// プレイヤーのインスタンス取得
	Player& player = Player::Instance();
	DirectX::XMFLOAT3 pos;
	pos.x = player.GetCollisionPos().x;
	pos.y = player.GetCollisionPos().y + 3;
	pos.z = player.GetCollisionPos().z;

	// 位置を設定
	position = pos;

	int bananaCount = ImportantItemManager::Instance().GetImportantItemCount();
	int enemyCount = EnemyManager::Instance().GetEnemyCount();

	// バナナが生成されているなら
	if (bananaCount > 0)
	{
		std::unique_ptr<ImportantItem>& banana = ImportantItemManager::Instance().GetImportantItem(0);

		// 現在の向きベクトル（すでに正規化されている前提）
		DirectX::XMFLOAT3 currentDir = direction;

		// ターゲットの向きベクトルを計算
		DirectX::XMFLOAT3 targetDir;
		targetDir.x = banana->GetCollisionPos().x - position.x;
		targetDir.y = 0;
		targetDir.z = banana->GetCollisionPos().z - position.z;

		/// XMVECTOR型に変換して正規化する
		DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetDir);
		targetVec = DirectX::XMVector3Normalize(targetVec);

		// 現在の向きをXMVECTOR型に変換
		DirectX::XMVECTOR currentVec = DirectX::XMLoadFloat3(&currentDir);

		// 球面線形補間で滑らかに方向を変える（tは補間係数。小さいほどゆっくり変わる）
		float t = 0.1f;  // 回転速度（調整可能）
		DirectX::XMVECTOR smoothVec = DirectX::XMVectorLerp(currentVec, targetVec, t);

		// 結果を格納
		DirectX::XMStoreFloat3(&direction, smoothVec);
	}
	else if (player.GetBananaNum() >= 6 && enemyCount > 0)
	{
		std::unique_ptr<Enemy>& sikaTentyo = EnemyManager::Instance().GetEnemy(0);

		// 現在の向きベクトル（すでに正規化されている前提）
		DirectX::XMFLOAT3 currentDir = direction;

		// ターゲットの向きベクトルを計算
		DirectX::XMFLOAT3 targetDir;
		targetDir.x = sikaTentyo->GetCollisionPos().x - position.x;
		targetDir.y = 0;
		targetDir.z = sikaTentyo->GetCollisionPos().z - position.z;

		/// XMVECTOR型に変換して正規化する
		DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetDir);
		targetVec = DirectX::XMVector3Normalize(targetVec);

		// 現在の向きをXMVECTOR型に変換
		DirectX::XMVECTOR currentVec = DirectX::XMLoadFloat3(&currentDir);

		// 球面線形補間で滑らかに方向を変える（tは補間係数。小さいほどゆっくり変わる）
		float t = 0.1f;  // 回転速度（調整可能）
		DirectX::XMVECTOR smoothVec = DirectX::XMVectorLerp(currentVec, targetVec, t);

		// 結果を格納
		DirectX::XMStoreFloat3(&direction, smoothVec);
	}

	// オブジェクト行列を更新
	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void YazirusiPlayer::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

// 発射
void YazirusiPlayer::Launch(const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}