cbuffer mycBuffer : register(b0)
{
    // row_major : 행 우선
    // column_major : 열 우선
    // a11 a12 a13 a14
    // a21 a22 a23 a24
    // a31 a32 a33 a34
    // a41 a42 a43 a44
    // 이런 상황인 경우 하나의 온전한 col을 저장한 뒤 다음 col을 저장하는 방식이다.
    // C언어 같은 경우 row-major이기 때문에 메모리 접근 시 다음과 같이 할 것이다.
    // 1 2 3 4
    // 5 6 7 8
    // 9 10 11 12
    // 13 14 15 16
    // 하지만 HLSL같은 col-major시 다음과 같이 접근 연산을 한다.
    // 1 5 9  13
    // 2 6 10 14
    // 3 7 11 15
    // 4 8 12 16
    // column_major float4x4 mat;       // 매트릭스
    float4x4 mat; // 매트릭스
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

    // 출력값을 설정한 뒤 이를 반영한다.
    output.outPosition = mul(float4(input.inPos, 1.0f), mat);
    output.outTexCoord = input.inTexCoord;

    return output;
}