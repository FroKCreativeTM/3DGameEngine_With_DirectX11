#include "CAdapterReader.h"

vector<CAdapterData> CAdapterReader::m_adapters;

CAdapterData::CAdapterData(IDXGIAdapter* pAdapter)
{
    this->m_pAdapter = pAdapter;
    HRESULT hr = pAdapter->GetDesc(&this->m_despcription);

    if (FAILED(hr)) 
    {
        throw(CGameError(NSGameError::FATAL_ERROR, "Error CAdapterData::CAdapterData() pAdapter->GetDesc()"));
        exit(-1);
    }
}

vector<CAdapterData> CAdapterReader::GetAdapters()
{
    // �̹� ��� �����Ͱ� �ʱ�ȭ�� �Ǿ��ٸ�
    if (m_adapters.size() > 0) 
    {
        return m_adapters;
    }

    Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

    // DXGIFactory object�� �����Ѵ�.
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), 
        reinterpret_cast<void**>(pFactory.GetAddressOf()));

    if(FAILED(hr)) 
    {
        throw(CGameError(NSGameError::FATAL_ERROR, "Error CAdapterReader::GetAdapters() CreateDXGIFactory()"));
        exit(-1);
    }

    IDXGIAdapter* pAdapter;
    UINT idx = 0;

    while (SUCCEEDED(pFactory->EnumAdapters(idx, &pAdapter))) 
    {
        m_adapters.push_back(CAdapterData(pAdapter));
        idx++;
    }

    return m_adapters;
}
