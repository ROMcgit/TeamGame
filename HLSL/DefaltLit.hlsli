struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL; // �@��
    float3 worldPosition : POSITION; // ���[���h���W
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection; // ���C�g�̌���
    float4 lightColor; // ���C�g�̐F
    float3 lightPosition; // �J�����̈ʒu
    float lightRange; // ���C�g�͈̔�
    
    float ambientStrength; // ����
    float diffuseStrength; // �g�U��
    float specularStrength; // �X�y�L�����[��
    float lightPadding; // ���C�g�̃p�f�B���O
    
    float4 fogColor; // �t�H�O�̐F
    float fogStart; // �t�H�O�̊J�n
    float fogEnd; // �t�H�O�̏I��
    float2 fogPadding; // �t�H�O�̃p�f�B���O
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
    float3 emissiveColor;
    float emissiveStrength;
};