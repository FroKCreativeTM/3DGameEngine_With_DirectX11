#include "CGraphics.h"

#include <ctime>

bool CGraphics::initD3DApp(HWND hwnd, int width, int height)
{
	vector<CAdapterData> adapters = CAdapterReader::GetAdapters();

	if (adapters.size() < 1) 
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() adapters.size() < 1 \n \
					No DXGI Adapters found"));
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 144;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(	adapters[0].m_pAdapter,					// ����� IDXGI Adapter
												D3D_DRIVER_TYPE_UNKNOWN,				// ���� �������� ���� ����̹� Ÿ��
												NULL,									// ����Ʈ���� ����̹� Ÿ��
												NULL,									// ��Ÿ�� ���̾ ���� �÷���
												NULL,									// ��ó ���� �迭�� ���� 
												0,										// �迭 ���� ��ó ������ #
												D3D11_SDK_VERSION,
												&scd,									// ���� ü�� ��ũ����(���� ����ü�� ���� �ȴ�)
												this->m_pSwapChain.GetAddressOf(),		// ����ü�� ������ �ּ�
												this->m_pDevice.GetAddressOf(),			// ����̽� �ּ�
												NULL,									// �����Ǵ� ��ó����
												this->m_pDeviceContext.GetAddressOf());	// ����̽� ���ؽ�Ʈ �ּ�

	if(FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() D3D11CreateDeviceAndSwapChain()"));
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() m_pSwapChain->GetBuffer()"));
		return false;
	}

	hr = this->m_pDevice->CreateRenderTargetView(
		backBuffer.Get(),
		NULL,
		this->m_pRenderingTargetView.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() CreateRenderTargetView()"));
		return false;
	}

	this->m_pDeviceContext->OMSetRenderTargets(
		1,
		this->m_pRenderingTargetView.GetAddressOf(),
		NULL);

	return true;
}

bool CGraphics::initializeShader()
{
	if (!m_pVertexShader.Initialize(m_pDevice, L"..\\Bin\\VertexShader.cso"))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// 1. sementic �̸� : ���̴��� �Է� �ñ׳�ó ���� �� ��ҿ� ���õ� HLSL�� �̸�
		// 2. sement idx 
		// 3. DXGI_FORMAT : ���� Ÿ�� ���
		// 4. InputSlot
		// 5. AlignedByteOffset : �� �״�� ������
		// 6. InputSlotClass : 
		// 7. InstanceDataStepRate
		{"POSITION", 0,
		DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
		0,0,
		D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	UINT numElements = ARRAYSIZE(layout);

	HRESULT hr = this->m_pDevice->CreateInputLayout(layout,
													numElements,
													this->m_pVertexShader.GetBuffer()->GetBufferPointer(),
													this->m_pVertexShader.GetBuffer()->GetBufferSize(),
													this->m_pInputLayout.GetAddressOf());

	if (FAILED(hr)) 
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeShader() \
				m_pDevice->CreateInputLayout()"));
		return false;
	}

	return true;
}

CGraphics::CGraphics()
{
}

CGraphics::~CGraphics()
{
}

bool CGraphics::Initialize(HWND hwnd, int width, int height)
{
	if (!initD3DApp(hwnd, width, height)) 
	{
		return false;
	}

	if (!initializeShader()) 
	{
		return false;
	}

	return true;
}

void CGraphics::Render(float fDeltaTime)
{
	float bgColor[] = { 0.0f, 0.0f, 1.0f ,1.0f};
	this->m_pDeviceContext->ClearRenderTargetView(this->m_pRenderingTargetView.Get(), bgColor);
	this->m_pSwapChain->Present(1, NULL);
}
