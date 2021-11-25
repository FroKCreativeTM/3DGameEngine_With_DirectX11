#pragma once

#include "../../Game.h"
#include "../../Error/CGameError.h"

class CVertexShader
{
public : 
	// �ʱ�ȭ�� �� �� Inputlayout�� ����ȴ�.
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, wstring shaderPath,
		D3D11_INPUT_ELEMENT_DESC * pLayoutDesc, UINT numElements);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();

private : 
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_pShader;
	Microsoft::WRL::ComPtr<ID3D10Blob>				m_pShaderBuffer;
	// �׷��� �������� �ʿ��� ��
	// Input ����� : ù ��° ������ ���� ���α׷����� ������ ���� ���۷κ��� 
	// ���� �����͸� �ٸ� ���������� �ܰ迡�� ����� ������Ƽ��� �����ϴ� ��
	// �� ��° ������ �ý��� ���� ��(System-generated-Value)�� �߰��ϴ� ��
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_pInputLayout;
};

