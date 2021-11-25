// float2 : XMFLOAT2 를 쓰기 때문에 input도 그렇게 받아야한다.
float4 main( float2 inPos : POSITION ) : SV_POSITION
{
    return float4(inPos, 0, 1);
}