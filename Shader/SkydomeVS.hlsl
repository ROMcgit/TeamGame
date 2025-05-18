//skydomeVS.hlsl
#include "Skydome.hlsli"

//!�p�xX
float4x4 RotationX(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return float4x4(
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
    );
}

// �p�xY
float4x4 RotationY(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return float4x4(
        c, 0, -s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1
    );
}

// �p�xZ
float4x4 RotationZ(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return float4x4(
         c, s, 0, 0,
        -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

VS_OUT main(
	float4 position : POSITION,
	float2 texcoord : TEXCOORD
)
{
    VS_OUT vout = (VS_OUT) 0;
   
    // �傫��
    position.xyz *= skydomeScale;
    
    // ��]�s���XYZ�����iZ��Y��X�̏��ŉ񂷂̂���ʓI�j
    float4x4 rotX = RotationX(skydomeAngle.x);
    float4x4 rotY = RotationY(skydomeAngle.y);
    float4x4 rotZ = RotationZ(skydomeAngle.z);
    
    float4x4 rot = mul(rotZ, mul(rotY, rotX)); // ��]������
    position = mul(position, rot);
    
    // �ʒu�����Z
    position.xyz += skydomePosition;
    
    // �J�����Ɉ����t���� 
    position += cameraPosition;
   
    vout.position = mul(position, viewProjection);
    vout.texcoord = texcoord;
    return vout;
}