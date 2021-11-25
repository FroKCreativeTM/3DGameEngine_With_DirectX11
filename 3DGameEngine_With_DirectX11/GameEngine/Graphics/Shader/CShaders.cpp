#include "CShaders.h"

bool CVertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, wstring shaderPath)
{
    HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_pShaderBuffer.GetAddressOf());

    if (FAILED(hr)) 
    {
        wstring errorMsg = L"Failed to load Shader";
        errorMsg += shaderPath;
        throw(CGameError(NSGameError::FATAL_ERROR, hr, errorMsg));
        return false;
    }
    
    hr = device->CreateVertexShader(this->m_pShaderBuffer->GetBufferPointer(),
        this->m_pShaderBuffer->GetBufferSize(),
        NULL,
        this->m_pShader.GetAddressOf());

    if (FAILED(hr))
    {
        wstring errorMsg = L"Failed to load Shader";
        errorMsg += shaderPath;
        throw(CGameError(NSGameError::FATAL_ERROR, hr, errorMsg));
        return false;
    }

    return true;
}

ID3D11VertexShader* CVertexShader::GetShader()
{
    return this->m_pShader.Get();
}

ID3D10Blob* CVertexShader::GetBuffer()
{
    return this->m_pShaderBuffer.Get();
}
