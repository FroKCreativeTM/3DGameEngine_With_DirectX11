#pragma once

#include "../../Game.h"
#include "../../Error/CGameError.h"

class CVertexShader
{
public : 
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, wstring shaderPath);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();

private : 
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_pShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob>				m_pShaderBuffer = nullptr;
};

