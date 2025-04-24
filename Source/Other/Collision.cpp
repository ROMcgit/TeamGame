#include "Collision.h"

// 球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A→Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// 距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}

	// AがBを押し出す
	DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(Vec);
	DirectX::XMVECTOR pushOutVec = DirectX::XMVectorScale(direction, sqrtf(lengthSq) - range);
	DirectX::XMVECTOR newPositionB = DirectX::XMVectorAdd(PositionB, pushOutVec);
	DirectX::XMStoreFloat3(&outPositionB, newPositionB);

	return true;
}

bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	// Aの足元がBの頭より上なら当たっていない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}

	// Aの頭がBの足元より下なら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}

	/// XZ平面での範囲チェック
	float dx = positionA.x - positionB.x;
	float dz = positionA.z - positionB.z;
	float distanceSquared = dx * dx + dz * dz; //正規化
	float radiusSum = radiusA + radiusB;

	if (distanceSquared > radiusSum * radiusSum)
	{
		return false;
	}

	//AがBを押し出す
	float distance = sqrtf(distanceSquared);
	float overlap = radiusSum - distance;

	// 正規化
	float normX = dx / distance;
	float normZ = dz / distance;

	outPositionB.x = positionB.x - normX * overlap;
	outPositionB.z = positionB.z - normZ * overlap;
	// Y座標は変わらない
	outPositionB.y = positionB.y;

	return true;
}

// 球と円柱の交差判定
bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition,
	float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
{
	// 円柱の底面中心から球の中心へのベクトル
	DirectX::XMFLOAT3 delta = {
		spherePosition.x - cylinderPosition.x,
		0.0f, // Y方向は無視
		spherePosition.z - cylinderPosition.z
	};

	/// XZ平面での距離の2乗
	float distanceXZ2 = delta.x * delta.x + delta.z * delta.z;
	float radiusSum = sphereRadius + cylinderRadius;


	/// XZ平面での距離が、球の半径と円柱の半径の和以下なら交差
	if (distanceXZ2 <= radiusSum * radiusSum) {
		// 球の中心が円柱の高さ範囲内にあるか確認
		float sphereBottom = spherePosition.y - sphereRadius;
		float sphereTop = spherePosition.y + sphereRadius;
		float cylinderBottom = cylinderPosition.y;
		float cylinderTop = cylinderPosition.y + cylinderHeight;

		if (sphereTop >= cylinderBottom && sphereBottom <= cylinderTop) {
			// 交差している場合、交差位置を計算
			outCylinderPosition = cylinderPosition;
			return true;
		}
	}

	return false;
}

// 球とボックスの交差判定
bool Collision::IntersectSphereVsBox(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& boxPosition, float boxWidth, float boxHeight, float boxDepth, DirectX::XMFLOAT3& outBoxPosition)
{
	// ボックスの最小・最大座標を計算
	DirectX::XMFLOAT3 boxMin = {
		boxPosition.x - boxWidth * 0.5f,
		boxPosition.y - boxHeight * 0.5f,
		boxPosition.z - boxDepth * 0.5f
	};

	DirectX::XMFLOAT3 boxMax = {
		boxPosition.x + boxWidth * 0.5f,
		boxPosition.y + boxHeight * 0.5f,
		boxPosition.z + boxDepth * 0.5f
	};

	// ボックス表面上の最近接点を求める
	DirectX::XMFLOAT3 closestPoint = {
		std::max<float>(boxMin.x, std::min<float>(spherePosition.x, boxMax.x)),
		std::max<float>(boxMin.y, std::min<float>(spherePosition.y, boxMax.y)),
		std::max<float>(boxMin.z, std::min<float>(spherePosition.z, boxMax.z))
	};

	// 球の中心と最近接点の距離を計算
	float deltaX = spherePosition.x - closestPoint.x;
	float deltaY = spherePosition.y - closestPoint.y;
	float deltaZ = spherePosition.z - closestPoint.z;
	float distanceSquared = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

	// 球の半径の2乗と比較して交差判定
	if (distanceSquared <= sphereRadius * sphereRadius) {
		// outBoxPosition に最近接点を格納
		outBoxPosition = closestPoint;
		return true;
	}

	return false;
}

// ボックスと円柱の交差判定
bool Collision::IntersectBoxVsCylinder(
	const DirectX::XMFLOAT3& boxPosition,
	float boxWidth, float boxHeight, float boxDepth,
	const DirectX::XMFLOAT3& cylPosition,
	float cylRadius, float cylHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
{
	// ■■ 垂直方向の重なり判定 ■■
	// ボックスのY範囲
	float boxMinY = boxPosition.y - boxHeight * 0.5f;
	float boxMaxY = boxPosition.y + boxHeight * 0.5f;
	// 円柱のY範囲（円柱は底面中心から高さ分）
	float cylMinY = cylPosition.y;
	float cylMaxY = cylPosition.y + cylHeight;

	// 垂直方向で重なっていなければ衝突なし
	if (cylMinY > boxMaxY || cylMaxY < boxMinY)
	{
		return false;
	}

	// ■■ 水平方向（XZ平面）の判定 ■■
	// ボックスのXZ平面での範囲
	float boxMinX = boxPosition.x - boxWidth * 0.5f;
	float boxMaxX = boxPosition.x + boxWidth * 0.5f;
	float boxMinZ = boxPosition.z - boxDepth * 0.5f;
	float boxMaxZ = boxPosition.z + boxDepth * 0.5f;

	// 円柱のXZ平面上の中心
	float cx = cylPosition.x;
	float cz = cylPosition.z;

	// 円（円柱の底面）と長方形（ボックスの水平投影）の交差判定
	// まず、円の中心からボックス内で最も近い点（クランプ）を求める
	float closestX = (cx < boxMinX) ? boxMinX : ((cx > boxMaxX) ? boxMaxX : cx);
	float closestZ = (cz < boxMinZ) ? boxMinZ : ((cz > boxMaxZ) ? boxMaxZ : cz);

	// 円の中心からその最も近い点までの距離（XZ平面）
	float dx = cx - closestX;
	float dz = cz - closestZ;
	float distSq = dx * dx + dz * dz;

	// 衝突していなければ（円の外側）false
	if (distSq > cylRadius * cylRadius)
	{
		return false;
	}

	// ■■ 衝突している場合：円柱を押し出す（円柱の新しいXZ位置を算出） ■■
	if (distSq == 0.0f)
	{
		// 円の中心がボックス内部にある場合
		// ボックスの各辺までの距離を計算し、最小のものを採用して押し出し方向を決定
		float distLeft = cx - boxMinX;
		float distRight = boxMaxX - cx;
		float distFront = cz - boxMinZ;
		float distBack = boxMaxZ - cz;

		// 最小の距離を求める
		float minDist = distLeft;
		// 初期の押し出し方向：正X方向
		DirectX::XMFLOAT3 pushDir = { 1.0f, 0.0f, 0.0f };

		if (distRight < minDist)
		{
			minDist = distRight;
			pushDir = { -1.0f, 0.0f, 0.0f };
		}
		if (distFront < minDist)
		{
			minDist = distFront;
			pushDir = { 0.0f, 0.0f, 1.0f };
		}
		if (distBack < minDist)
		{
			minDist = distBack;
			pushDir = { 0.0f, 0.0f, -1.0f };
		}

		// 押し出す量は、円柱の半径からその辺までの距離
		float penetration = cylRadius - minDist;
		// 新しい円柱のXZ位置
		outCylinderPosition.x = cylPosition.x + pushDir.x * penetration;
		outCylinderPosition.z = cylPosition.z + pushDir.z * penetration;
	}
	else
	{
		// 円柱の中心がボックス外部にある場合
		float dist = sqrtf(distSq);
		float penetration = cylRadius - dist;

		// 押し出し方向は、円柱中心からクランプした点への正規化ベクトル
		DirectX::XMVECTOR norm = DirectX::XMVectorSet(dx, 0.0f, dz, 0.0f);
		norm = DirectX::XMVector3Normalize(norm);
		DirectX::XMVECTOR pushVec = DirectX::XMVectorScale(norm, penetration);
		DirectX::XMFLOAT3 pushOut;
		DirectX::XMStoreFloat3(&pushOut, pushVec);

		outCylinderPosition.x = cylPosition.x + pushOut.x;
		outCylinderPosition.z = cylPosition.z + pushOut.z;
	}

	// Y座標はそのまま（円柱の底面位置は変えない）
	outCylinderPosition.y = cylPosition.y;

	return true;
}

// レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const Model* model,
	HitResult& result)
{
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	// ワールド空間のレイの長さ
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resouce = model->GetResource();
	for (const ModelResource::Mesh& mesh : resouce->GetMeshes())
	{
		// メッシュノード取得
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// レイをワールド空間からローカル空間へ変換
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

		// レイの長さ
		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		// 三角形(面)との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				// 三角形の頂点を抽出
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				// 三角形の三辺ベクトルを算出
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

				// 三角形の法線ベクトルを算出
				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

				// 内積の結果がプラスなら裏向き
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
				float d;
				DirectX::XMStoreFloat(&d, Dot);

				if (d >= 0.0f) continue;

				// レイと平面の交差を算出
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N, SA), Dot);

				float x;
				DirectX::XMStoreFloat(&x, X);
				if (x < 0 || x > neart) continue;

				DirectX::XMVECTOR P = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(V, X), S);

				// 交点が三角形の内側にあるか判定
				// 1つめ
				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);

				if (dot1 < 0) continue;
				// 2つめ
				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);

				if (dot2 < 0) continue;
				// 3つめ
				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);

				if (dot3 < 0) continue;

				// 最短距離を更新
				// 交点と法線を更新
				HitPosition = P;
				HitNormal = N;
				materialIndex = subset.materialIndex;
			}
		}
		if (materialIndex >= 0)
		{
			// ローカル空間からワールド空間へ変換
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition,
				WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			// ヒット情報保存
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal,
					WorldTransform);
				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}
	return hit;
}