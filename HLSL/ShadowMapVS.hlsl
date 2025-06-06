#include "ShadowMap.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices : BONES
)
{
    float3 p = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
    }

    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), shadowProjection);
    vout.distance = vout.position.z;
    
    return vout;
}
