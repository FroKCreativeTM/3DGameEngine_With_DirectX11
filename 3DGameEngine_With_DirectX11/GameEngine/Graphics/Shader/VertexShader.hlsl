cbuffer mycBuffer : register(b0)
{
    // row_major : �� �켱
    // column_major : �� �켱
    // a11 a12 a13 a14
    // a21 a22 a23 a24
    // a31 a32 a33 a34
    // a41 a42 a43 a44
    // �̷� ��Ȳ�� ��� �ϳ��� ������ col�� ������ �� ���� col�� �����ϴ� ����̴�.
    // C��� ���� ��� row-major�̱� ������ �޸� ���� �� ������ ���� �� ���̴�.
    // 1 2 3 4
    // 5 6 7 8
    // 9 10 11 12
    // 13 14 15 16
    // ������ HLSL���� col-major�� ������ ���� ���� ������ �Ѵ�.
    // 1 5 9  13
    // 2 6 10 14
    // 3 7 11 15
    // 4 8 12 16
    // column_major float4x4 mat;       // ��Ʈ����
    float4x4 mat; // ��Ʈ����
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

// float2 : XMFLOAT2 �� ���� ������ input�� �׷��� �޾ƾ��Ѵ�.
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // ��°��� ������ �� �̸� �ݿ��Ѵ�.
    output.outPosition = mul(float4(input.inPos, 1.0f), mat);
    output.outTexCoord = input.inTexCoord;

    return output;
}