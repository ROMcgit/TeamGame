#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Graphics/ModelResource.h"

// モデル
class Model
{
public:
	Model(const char* filename);
	~Model() {}

	struct Node
	{
		const char* name;
		Node* parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	localTransform;
		DirectX::XMFLOAT4X4	worldTransform;

		std::vector<Node*>	children;
	};

	/*! ゲッター */

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

	// ノード検索
	Node* FindNode(const char* name);

	// アニメーション再生中か
	bool IsPlayAnimation() const;

	// アニメーション番号を取得
	int GetAnimationNum() { return animationNum; }

	// アニメーションの時間を取得
	float GetCurrentAnimationSeconds() const { return currentAnimationSeconds; }

	/*! セッター */

	// アニメーション再生
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f);

	/*****************************************************************************************************************/
		/*! 更新処理 */

		// 行列計算
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);

private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;

	int currentAnimationIndex = -1;
	float currentAnimationSeconds = 0.0f;

	bool animationLoopFlag = false;
	bool animationEndFlag = false;

	float animationBlendTime = 0.0f;
	float animationBlendSeconds = 0.0f;

	int animationNum = 0;
};