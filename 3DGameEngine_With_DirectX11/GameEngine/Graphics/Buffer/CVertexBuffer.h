#pragma once

#include "../../Game.h"

template <typename T>
class CVertexBuffer
{
private : 
	// 복사 생성자는 막는다.
	CVertexBuffer(const CVertexBuffer<T>& rhs);

private :
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pBuffer;
	std::unique_ptr<UINT>					m_pStride;
	UINT									m_bufferSize;

public : 
	CVertexBuffer() {}

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
	
	const UINT GetStride() const
	{
		return *this->m_pStride.get();
	}

	const UINT* GetStridePtr() const
	{
		return this->m_pStride.get();
	}

	HRESULT Initialize(ID3D11Device* pDevice, T* data, UINT nVertices)
	{
		this->m_bufferSize = nVertices;
		this->m_pStride = std::make_unique<UINT>(sizeof(T));

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(T) * nVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		vertexBufferData.pSysMem = data;

		HRESULT hr = pDevice->CreateBuffer(&vertexBufferDesc,
			&vertexBufferData,
			this->m_pBuffer.GetAddressOf());
		return hr;
	}
};

