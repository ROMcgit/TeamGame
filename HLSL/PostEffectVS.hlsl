//PostEffectVS
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUT main(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    vout.position = position;
    vout.texcoord = texcoord;
    return vout;
}