#pragma once

#include "../../Game.h"
#include "../../Error/CGameError.h"
#include "ConstBufferType.h"

template <typename T>
class CConstantBuffer
{
private:
	// ���� �����ڴ� ���´�.
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
		desc.Usage = D3D11_USAGE_DYNAMIC;				// �츮�� ��ȭ��ų �� �ְ� ���̳����ϰ� ����
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// ��� ���۸� ���ε�
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU�� �����ؼ� ���� ���� ����������.
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));	// ��ü ����� 16 ����̴�.
		desc.StructureByteStride = 0;

		HRESULT hr = pDevice->CreateBuffer(&desc,
			0,
			this->m_pBuffer.GetAddressOf());
		return hr;
	}

	bool ApplyChanges()
	{
		// ��� ���ۿ� ���� ������Ʈ�� �����Ѵ�.
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

