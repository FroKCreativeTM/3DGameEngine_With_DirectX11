#include "CShaders.h"

bool CVertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, wstring shaderPath,
    D3D11_INPUT_ELEMENT_DESC* pLayoutDesc, UINT numElements)
{
    HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_pShaderBuffer.GetAddressOf());

    if (FAILED(hr)) 
    {
        wstring errorMsg = L"Failed to load Shader : ";
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
        std::wstring errorMsg = L"Failed to create vertex shader: ";
        errorMsg += shaderPath;
        throw(CGameError(NSGameError::FATAL_ERROR, hr, errorMsg));
        return false;
    }

    hr = device->CreateInputLayout(pLayoutDesc,
        numElements,
        this->m_pShaderBuffer->GetBufferPointer(),
        this->m_pShaderBuffer->GetBufferSize(),
        this->m_pInputLayout.GetAddressOf());

    if (FAILED(hr))
    {
        throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeShader() \
				m_pDevice->CreateInputLayout()"));
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

ID3D11InputLayout* CVertexShader::GetInputLayout()
{
    return this->m_pInputLayout.Get();
}

bool CPixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, wstring shaderPath)
{
    HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->m_pShaderBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        wstring errorMsg = L"Failed to load Shader : ";
        errorMsg += shaderPath;
        throw(CGameError(NSGameError::FATAL_ERROR, hr, errorMsg));
        return false;
    }

    hr = device->CreatePixelShader(this->m_pShaderBuffer.Get()->GetBufferPointer(), 
        this->m_pShaderBuffer.Get()->GetBufferSize(),
        NULL, 
        this->m_pShader.GetAddressOf());

    if (FAILED(hr))
    {
        std::wstring errorMsg = L"Failed to create pixel shader: ";
        errorMsg += shaderPath;
        throw(CGameError(NSGameError::FATAL_ERROR, hr, errorMsg));
        return false;
    }

    return true;
}

ID3D11PixelShader* CPixelShader::GetShader() 
{
    return this->m_pShader.Get();
}

ID3D10Blob* CPixelShader::GetBuffer()
{
    return this->m_pShaderBuffer.Get();
}
