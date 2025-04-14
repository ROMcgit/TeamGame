//ShadowMap.hlsli

// �V���h�E�}�b�v�p�R���X�^���g�o�b�t�@
cbuffer CbShadowMap : register(b10)
{
    row_major float4x4 shadowProjection;
}

// ���_�V�F�[�_�[�@���@�s�N�Z���V�F�[�_�[�ɓn���l
struct VS_OUT
{
    float4 position : SV_POSITION;
    float distance : TEXCOORD0;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};