#include "./GameEngine/CCore.h"

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	HRESULT hr = CoInitialize(nullptr);
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CoInitialize()"));
		return -1;
	}

	if (!CCore::GetInst()->Init(hInstance, 800, 600))
	{
		CCore::DestroyInst();
		return 0;
	}

	// return value
	int nRev = CCore::GetInst()->Run();
	CCore::DestroyInst();

	return nRev;
}