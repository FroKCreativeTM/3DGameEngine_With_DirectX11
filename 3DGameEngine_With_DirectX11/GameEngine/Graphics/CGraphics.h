#pragma once

#include "../Game.h"
#include "../Error/CGameError.h"
#include "CAdapterReader.h"
#include "Shader/CShaders.h"

class CGraphics
{
private : 
	// 엔진 내부에서만 사용하는 D3D 프레젠테이션 
	// 매개변수를 초기화한다.
	bool initD3DApp(HWND hwnd, int width, int height);

	// 셰이더를 초기화 하기 위한 함수이다.
	bool initializeShader();

public:
	// 생성자
	CGraphics();

	// 소멸자
	virtual ~CGraphics();

	// DirectX 그래픽을 초기화한다.
	bool Initialize(HWND hwnd, int width, int height);

	void Render(float fDeltaTime);

private:
	Microsoft::WRL::ComPtr<ID3D11Device>						m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>					m_pDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>						m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>				m_pRenderingTargetView;

	// 그래픽 렌더링에 필요한 것
	// Input 어셈블러 : 첫 번째 목적은 응용 프로그램에서 제공한 정점 버퍼로부터 
	// 정점 데이터를 다른 파이프라인 단계에서 사용할 프리미티브로 조립하는 것
	// 두 번째 목적은 시스템 생성 값(System-generated-Value)을 추가하는 것
	Microsoft::WRL::ComPtr<ID3D11InputLayout>					m_pInputLayout;
	// VERTEX SHADER
	CVertexShader												m_pVertexShader;
	// RESTERIZER
	// PIXEL SHADER
	// OUTPUT MERGER
};

