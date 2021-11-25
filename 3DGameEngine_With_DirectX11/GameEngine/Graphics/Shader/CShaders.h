#pragma once

#include "../../Game.h"
#include "../../Error/CGameError.h"

class CVertexShader
{
public : 
	// 초기화를 할 때 Inputlayout과 연결된다.
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, wstring shaderPath,
		D3D11_INPUT_ELEMENT_DESC * pLayoutDesc, UINT numElements);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();

private : 
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_pShader;
	Microsoft::WRL::ComPtr<ID3D10Blob>				m_pShaderBuffer;
	// 그래픽 렌더링에 필요한 것
	// Input 어셈블러 : 첫 번째 목적은 응용 프로그램에서 제공한 정점 버퍼로부터 
	// 정점 데이터를 다른 파이프라인 단계에서 사용할 프리미티브로 조립하는 것
	// 두 번째 목적은 시스템 생성 값(System-generated-Value)을 추가하는 것
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_pInputLayout;
};

