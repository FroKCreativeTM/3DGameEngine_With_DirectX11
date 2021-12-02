#pragma once

#include "../../Game.h"
#include "../../Error/CGameError.h"
#include "ConstBufferType.h"

template <typename T>
class CConstantBuffer
{
private:
	// 복사 생성자는 막는다.
	CConstantBuffer(const CConstantBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pBuffer;
	ID3D11DeviceContext*					m_pDeviceContext = nullptr;
	T										m_data;

public:
	CConstantBuffer() {}

	T& GetData() 
	{
		return m_data;
	}

	ID3D11Buffer* Get() const
	{
		return m_pBuffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const
	{
		return m_pBuffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		this->m_pDeviceContext = pDeviceContext;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;				// 우리가 변화시킬 수 있게 다이나믹하게 설정
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 상수 버퍼를 바인딩
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU에 접근해서 쓰는 것이 가능해진다.
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));	// 전체 사이즈가 16 배수이다.
		desc.StructureByteStride = 0;

		HRESULT hr = pDevice->CreateBuffer(&desc,
			0,
			this->m_pBuffer.GetAddressOf());
		return hr;
	}

	bool ApplyChanges()
	{
		// 상수 버퍼에 대해 업데이트를 진행한다.
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = m_pDeviceContext->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (FAILED(hr)) 
		{
			throw(CGameError(NSGameError::FATAL_ERROR, "Error CConstantBuffer ApplyChanges()"));
			return false;
		}

		CopyMemory(mappedResource.pData, &m_data, sizeof(T));
		m_pDeviceContext->Unmap(m_pBuffer.Get(), 0);
		return true;
	}
};

