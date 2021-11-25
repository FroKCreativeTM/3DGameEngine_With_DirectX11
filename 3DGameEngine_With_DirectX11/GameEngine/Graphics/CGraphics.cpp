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
	scd.BufferDesc.RefreshRate.Numerator = 60;
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

	// Output Merger�� ���
	this->m_pDeviceContext->OMSetRenderTargets(
		1,
		this->m_pRenderingTargetView.GetAddressOf(),
		NULL);

	// ���� ����Ʈ�� ���� ������ �Ѵ�.
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;

	// ����Ʈ�� �����Ѵ�.
	this->m_pDeviceContext->RSSetViewports(1, &viewport);
	 
	return true;
}

bool CGraphics::initializeShader()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// 1. sementic �̸� : ���̴��� �Է� �ñ׳�ó ���� �� ��ҿ� ���õ� HLSL�� �̸�
		// 2. sement idx 
		// 3. DXGI_FORMAT : ���� Ÿ�� ���
		// 4. InputSlot
		// 5. AlignedByteOffset : �� �״�� ������
		// 6. InputSlotClass : 
		// 7. InstanceDataStepRate

		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements = ARRAYSIZE(layout);

	// ���� ���̴� ���н� false, �ƴϸ� true
	if (!m_pVertexShader.Initialize(m_pDevice, L"..\\Bin\\VertexShader.cso",
		layout, numElements))
	{
		return false;
	}

	// �ȼ� ���̴� ���н� false, �ƴϸ� true
	if (!m_pPixelShader.Initialize(m_pDevice, L"..\\Bin\\PixelShader.cso"))
	{
		return false;
	}

	return true;
}

bool CGraphics::initializeScene()
{
	Vertex vertices[] =
	{
		Vertex(0.0f, 0.0f),		// center
		Vertex(-0.1f, 0.0f),	// left
		Vertex(0.1f, 0.0f),		// right
		Vertex(0.0f, 0.1f),		// top
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = vertices;

	HRESULT hr = this->m_pDevice->CreateBuffer(&vertexBufferDesc, 
		&vertexBufferData, 
		this->m_pVertexBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() m_pDevice->CreateBuffer()"));
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

	if (!initializeScene())
	{
		return false;
	}

	return true;
}

void CGraphics::Render(float fDeltaTime)
{
	float bgColor[] = { 0.0f, 0.0f, 1.0f ,1.0f};
	this->m_pDeviceContext->ClearRenderTargetView(this->m_pRenderingTargetView.Get(), bgColor);

	this->m_pDeviceContext->IASetInputLayout(this->m_pVertexShader.GetInputLayout());
	this->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	this->m_pDeviceContext->VSSetShader(m_pVertexShader.GetShader(), NULL, 0);
	this->m_pDeviceContext->PSSetShader(m_pPixelShader.GetShader(), NULL, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	this->m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	this->m_pDeviceContext->Draw(4, 0);

	this->m_pSwapChain->Present(1, NULL);
}
