#pragma once

#include "../Game.h"
#include "../Error/CGameError.h"
#include "CAdapterReader.h"
#include "Shader/CShaders.h"
#include "Buffer/CVertexBuffer.h"
#include "Buffer/CIndexBuffer.h"
#include "Buffer/CConstantBuffer.h"
#include "../Camera/CCamera.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

class CGraphics
{
private : 
	// ���� ���ο����� ����ϴ� D3D ���������̼� 
	// �Ű������� �ʱ�ȭ�Ѵ�.
	bool initD3DApp(HWND hwnd);

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
	int															m_nWindowWidth;
	int															m_nWindowHeight;

	Microsoft::WRL::ComPtr<ID3D11Device>						m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>					m_pDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>						m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>				m_pRenderingTargetView;

	// Input Assembler(�̰� Shader.h�� �ִ� layout�� �Ҳ���) (Complete)
	// VERTEX SHADER (Complete)
	CVertexShader												m_pVertexShader;			// ���ؽ� ���̴��� �ٷ�� ����
	CVertexBuffer<Vertex>										m_pVertexBuffer;			// ���ؽ��� �� ���۸� ����
	CIndexBuffer												m_pIndexBuffer;				// ���ۿ� �� ������ �ε����ؼ� �ٷ�� ���� ����
	CConstantBuffer<CB_VS_VERTEXSHADER>							m_pConstBufferVertexShader;	// ��� ����
	CConstantBuffer<CB_PS_PIXELSHADER>							m_pConstBufferPixelShader;	

	// RESTERIZER(Complete)
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>				m_pRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState>					m_pBlendState;
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
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>			m_pPinkTexture;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>			m_pGrassTexture;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>			m_pPavementTexture;
};

