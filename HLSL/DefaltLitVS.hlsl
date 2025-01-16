#include "DefaltLit.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices : BONES
)
{
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    float3 t = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
        t += (boneWeights[i] * mul(float4(tangent.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
    }

    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);

    float3 N = normalize(n);
    float3 L = normalize(-lightDirection.xyz);
    float3 T = normalize(t);
    float d = dot(L, N);
    float power = max(0, d) * 0.3f + 0.7f; // ライトの強さ
    
    // ライト色を反映した計算
    float3 lightContribution = lightColor.rgb * power;
    
    vout.color.rgb = color.rgb * materialColor.rgb * power;
    vout.color.a = color.a * materialColor.a;
    vout.texcoord = texcoord;

    vout.normal = N; // 法線をそのまま渡す
    vout.worldPosition = p; // ワールド空間の位置も渡す
    
    vout.tangent = T;
    vout.binormal = cross(vout.normal, vout.tangent);
    
    return vout;
}
