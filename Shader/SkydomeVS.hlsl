//skydomeVS.hlsl
#include "Skydome.hlsli"

//!角度X
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

// 角度Y
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

// 角度Z
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
   
    // 大きさ
    position.xyz *= skydomeScale;
    
    // 回転行列をXYZ合成（Z→Y→Xの順で回すのが一般的）
    float4x4 rotX = RotationX(skydomeAngle.x);
    float4x4 rotY = RotationY(skydomeAngle.y);
    float4x4 rotZ = RotationZ(skydomeAngle.z);
    
    float4x4 rot = mul(rotZ, mul(rotY, rotX)); // 回転を合成
    position = mul(position, rot);
    
    // 位置を加算
    position.xyz += skydomePosition;
    
    // カメラに引っ付ける 
    position += cameraPosition;
   
    vout.position = mul(position, viewProjection);
    vout.texcoord = texcoord;
    return vout;
}