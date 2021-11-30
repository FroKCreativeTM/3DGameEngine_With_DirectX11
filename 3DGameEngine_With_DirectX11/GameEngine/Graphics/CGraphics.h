#pragma once

#include "../Game.h"
#include "../Error/CGameError.h"
#include "CAdapterReader.h"
#include "Shader/CShaders.h"


class CGraphics
{
private : 
	// ���� ���ο����� ����ϴ� D3D ���������̼� 
	// �Ű������� �ʱ�ȭ�Ѵ�.
	bool initD3DApp(HWND hwnd, int width, int height);

	// ���̴��� �ʱ�ȭ �ϱ� ���� �Լ��̴�.
	bool initializeShader();

	// ���� �ʱ�ȭ �ϱ� ���� �Լ��̴�.
	bool initializeScene();

public:
	// ������
	CGraphics();

	// �Ҹ���
	virtual ~CGraphics();

	// DirectX �׷����� �ʱ�ȭ�Ѵ�.
	bool Initialize(HWND hwnd, int width, int height);

	void Render(float fDeltaTime);

private:
	Microsoft::WRL::ComPtr<ID3D11Device>						m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>					m_pDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>						m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>				m_pRenderingTargetView;

	// Input Assembler(�̰� Shader.h�� �ִ� layout�� �Ҳ���) (Complete)
	// VERTEX SHADER (Complete)
	CVertexShader												m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>						m_pVertexBuffer;
	// RESTERIZER(Complete)
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>				m_pRasterizerState;
	// PIXEL SHADER(Complete)
	CPixelShader												m_pPixelShader;
	// OUTPUT MERGER - OMSetRenderTargers(Complete)

	// ���� ���ۿ� ���� ����
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>				m_pDepthStencilView;	// ���ٽ� ���۴� ���� ����, �� ���ۿ� ���������� ���� ũ�⸦ ������ ���� ��ũ�� �����̴�.
																						// ������� �Ϻ� ������ �����Ͽ� �׸��� �ʱ� ���� ���Ǿ����� Ư���� ���۶�� �����ϸ� �ȴ�.
																						// ���� �ſ��̳� ��� �׸��ڿ� ���Ǹ�, �� �ſ￡ ��ġ�� �ʴ� ��ü�� �����ϱ� ���� �ִ� ���� �ٷ� �� ���ٽ� ����!
	Microsoft::WRL::ComPtr<ID3D11Texture2D>						m_pDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>				m_pDepthStencilState;

	std::unique_ptr<DirectX::SpriteBatch>						m_pSpriteBatch;
	std::unique_ptr<DirectX::SpriteFont>						m_pSpriteFont;

	// �ؽ�ó ���� ����
	Microsoft::WRL::ComPtr <ID3D11SamplerState>					m_pSamplerState;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>			m_pTexture;
};

