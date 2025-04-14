//ShadowMapPS.hlsl
#include "ShadowMap.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(pin.distance, pin.distance, pin.distance, 1.0);
}