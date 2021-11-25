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

	// Input Assembler(�̰� Shader.h�� �ִ� layout�� �Ҳ���)
	// VERTEX SHADER
	CVertexShader												m_pVertexShader;
	// RESTERIZER
	// PIXEL SHADER
	// OUTPUT MERGER
};

