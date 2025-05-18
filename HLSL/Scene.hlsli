cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    
    float4 cameraPosition; // �J�����̈ʒu
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