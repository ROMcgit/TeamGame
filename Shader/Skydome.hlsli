#include "Scene.hlsli"

// ���_�V�F�[�_�[�@���@�s�N�Z���V�F�[�_�[�ɓn���l
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// �傫��
cbuffer SkydomeTransform
{
    float3 skydomePosition;
    float  dummy;
    float3 skydomeAngle;
    float  skydomeScale;
};