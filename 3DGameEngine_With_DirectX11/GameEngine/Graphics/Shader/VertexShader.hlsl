cbuffer mycBuffer : register(b0)
{
    float xOffset;
    float yOffset;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
};

// float2 : XMFLOAT2 를 쓰기 때문에 input도 그렇게 받아야한다.
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // 먼저 상수 버퍼에 지정되어있는
    // 오프셋들을 x,y축으로 적용한다. 
    input.inPos.x += xOffset;
    input.inPos.y += yOffset;

    // 출력값을 설정한 뒤 이를 반영한다.
    output.outPosition = float4(input.inPos, 1.0f);
    output.outTexCoord = input.inTexCoord;

    return output;
}