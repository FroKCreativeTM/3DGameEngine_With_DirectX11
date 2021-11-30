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

	// 씬을 초기화 하기 위한 함수이다.
	bool initializeScene();

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

	// Input Assembler(이건 Shader.h에 있는 layout이 할꺼임) (Complete)
	// VERTEX SHADER (Complete)
	CVertexShader												m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>						m_pVertexBuffer;
	// RESTERIZER(Complete)
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>				m_pRasterizerState;
	// PIXEL SHADER(Complete)
	CPixelShader												m_pPixelShader;
	// OUTPUT MERGER - OMSetRenderTargers(Complete)

	// 깊이 버퍼에 대한 변수
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>				m_pDepthStencilView;	// 스텐실 버퍼는 깊이 버퍼, 백 버퍼와 마찬가지로 같은 크기를 가지는 오프 스크린 버퍼이다.
																						// 백버퍼의 일부 영역을 차단하여 그리지 않기 위해 사용되어지는 특수한 버퍼라고 생각하면 된다.
																						// 보통 거울이나 평면 그림자에 사용되며, 이 거울에 비치지 않는 물체를 차단하기 위해 있는 것이 바로 이 스텐실 버퍼!
	Microsoft::WRL::ComPtr<ID3D11Texture2D>						m_pDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>				m_pDepthStencilState;

	std::unique_ptr<DirectX::SpriteBatch>						m_pSpriteBatch;
	std::unique_ptr<DirectX::SpriteFont>						m_pSpriteFont;

	// 텍스처 관련 변수
	Microsoft::WRL::ComPtr <ID3D11SamplerState>					m_pSamplerState;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>			m_pTexture;
};

