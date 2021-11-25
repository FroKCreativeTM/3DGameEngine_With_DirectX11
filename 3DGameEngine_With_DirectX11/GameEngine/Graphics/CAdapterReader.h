#pragma once

#include "../Game.h"
#include "../Error/CGameError.h"

class CAdapterData
{
public :
	CAdapterData(IDXGIAdapter* pAdapter);

	IDXGIAdapter* m_pAdapter = nullptr;
	DXGI_ADAPTER_DESC			m_despcription;
};

class CAdapterReader
{
public : 
	static vector<CAdapterData> GetAdapters();
private : 
	static vector<CAdapterData> m_adapters;
};

