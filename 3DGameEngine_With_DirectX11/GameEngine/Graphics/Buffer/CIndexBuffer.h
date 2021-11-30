#pragma once

#include "../../Game.h"

class CIndexBuffer
{
private:
	// 복사 생성자는 막는다.
	CIndexBuffer(const CIndexBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pBuffer;
	UINT									m_bufferSize;

public:
	CIndexBuffer() {}

	ID3D11Buffer* Get() const
	{
		return m_pBuffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const
	{
		return m_pBuffer.GetAddressOf();
	}

	UINT GetBufferSize() const
	{
		return m_bufferSize;
	}

	HRESULT Initialize(ID3D11Device* pDevice, DWORD* data, UINT nVertices)
	{
		this->m_bufferSize = nVertices;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * nVertices;
		indexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;

		HRESULT hr = pDevice->CreateBuffer(&indexBufferDesc,
			&indexBufferData,
			this->m_pBuffer.GetAddressOf());
		return hr;
	}
};

