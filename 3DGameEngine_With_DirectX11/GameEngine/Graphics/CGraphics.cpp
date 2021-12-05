#include "CGraphics.h"

#include <ctime>

bool CGraphics::initD3DApp(HWND hwnd)
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

	scd.BufferDesc.Width = m_nWindowWidth;
	scd.BufferDesc.Height = m_nWindowHeight;
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

	// Depth/Stencil ���ۿ� ���� ����
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_nWindowWidth;
	depthStencilDesc.Height = m_nWindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->m_pDevice->CreateTexture2D(&depthStencilDesc, NULL, this->m_pDepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() CreateTexture2D()"));
		return false;
	}

	hr = this->m_pDevice->CreateDepthStencilView(this->m_pDepthStencilBuffer.Get(), NULL, this->m_pDepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() CreateTexture2D()"));
		return false;
	}

	// Output Merger�� ���
	this->m_pDeviceContext->OMSetRenderTargets(
		1,
		this->m_pRenderingTargetView.GetAddressOf(),
		this->m_pDepthStencilView.Get());

	// Depth Stencil state�� �����Ѵ�.
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	// ���� ���� Depth�� ������ �ִٸ� ���߿� ���� ���� ����.

	hr = this->m_pDevice->CreateDepthStencilState(&depthStencilStateDesc,
		this->m_pDepthStencilState.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() CreateDepthStencilState()"));
		return false;
	}

	// ���� ����Ʈ�� ���� ������ �Ѵ�.
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	// ����Ʈ ũ�� ����
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_nWindowWidth;
	viewport.Height = m_nWindowHeight;

	// ����Ʈ�� ���� ����
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// ����Ʈ�� �����Ѵ�.
	this->m_pDeviceContext->RSSetViewports(1, &viewport);
	 
	// �����Ͷ������� ���� ��ũ������ �����ϰ� �̸� �����Ͷ������� �����Ѵ�.
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	// rasterizerDesc.FrontCounterClockwise = TRUE;
	hr = this->m_pDevice->CreateRasterizerState(&rasterizerDesc, this->m_pRasterizerState.GetAddressOf());
	if (FAILED(hr)) 
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() m_pDevice->CreateRasterizerState()"));
		return false;
	}

	// ���� ���� ����
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	// Final Blend = Src Blend * Src Blend Factor + Dst Blend * Dst Blend Factor
	// Final Color = Src Pixel * Src Blend Factor + Dst Pixel * Dst Blend Factor
	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = rtbd;

	hr = this->m_pDevice->CreateBlendState(&blendDesc, m_pBlendState.GetAddressOf());
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() m_pDevice->CreateBlendState()"));
		return false;
	}

	// ��Ʈ�� ���� ����
	m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(this->m_pDeviceContext.Get());
	m_pSpriteFont = std::make_unique<DirectX::SpriteFont>(this->m_pDevice.Get(), L"GameData\\Fonts\\comic_sans_ms_16.spritefont");

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = this->m_pDevice->CreateSamplerState(
		&sampDesc,
		this->m_pSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() m_pDevice->CreateSamplerState()"));
		return false;
	}

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

		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
		D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
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
	// Z ���۸� ���� ���ؽ� 1
	// 2   3
	//
	// 1   4
	Vertex vertices[] =
	{
		Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f),		// bottom left 
		Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f),		// top left 
		Vertex(0.5f, 0.5f, 0.0f, 1.0f, 0.0f),		// top right 
		Vertex(0.5f, -0.5f, 0.0f, 1.0f, 1.0f),		// bottom right 
	}; 

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	// ���ؽ� ���۸� �����Ѵ�.
	HRESULT hr = this->m_pVertexBuffer.Initialize(this->m_pDevice.Get(), vertices, ARRAYSIZE(vertices));
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() m_pDevice->CreateBuffer() - Vertex"));
		return false;
	}

	// �ε��� ���۸� �����Ѵ�.
	hr = this->m_pIndexBuffer.Initialize(this->m_pDevice.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() m_pDevice->CreateBuffer() - Index"));
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->m_pDevice.Get(),
		L"GameData\\Textures\\todd_texture.jpg",
		nullptr,
		m_pTexture.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() DirectX::CreateWICTextureFromFile()"));
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->m_pDevice.Get(),
		L"GameData\\Textures\\pinksquare.jpg",
		nullptr,
		m_pPinkTexture.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() DirectX::CreateWICTextureFromFile()"));
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->m_pDevice.Get(),
		L"GameData\\Textures\\seamless_grass.jpg",
		nullptr,
		m_pGrassTexture.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() DirectX::CreateWICTextureFromFile()"));
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->m_pDevice.Get(),
		L"GameData\\Textures\\seamless_pavement.jpg",
		nullptr,
		m_pPavementTexture.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() DirectX::CreateWICTextureFromFile()"));
		return false;
	}

	// ��� ���۸� �ʱ�ȭ�Ѵ�
	hr = m_pConstBufferVertexShader.Initialize(this->m_pDevice.Get(), this->m_pDeviceContext.Get());
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() m_pConstBuffer.Initialize()"));
		return false;
	}

	hr = m_pConstBufferPixelShader.Initialize(this->m_pDevice.Get(), this->m_pDeviceContext.Get());
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() m_pConstBuffer.Initialize()"));
		return false;
	}

	GET_SINGLE(CCamera)->SetPosition(0.0f, 0.0f, -2.0f);
	GET_SINGLE(CCamera)->SetProjectionValues(90.0f, static_cast<float>(m_nWindowWidth) / static_cast<float>(m_nWindowHeight),
		0.1f, 1000.0f);

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
	m_nWindowWidth = width;
	m_nWindowHeight = height;

	if (!initD3DApp(hwnd)) 
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

	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pDeviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void CGraphics::Render(float fDeltaTime)
{
	float bgColor[] = { 0.0f, 0.0f, 0.0f ,1.0f};
	// Render Targer View�� Depth/Stencil View�� ����.
	this->m_pDeviceContext->ClearRenderTargetView(this->m_pRenderingTargetView.Get(), bgColor);
	// 3��°�� 1.0f�� MaxDepth
	// 4��°�� Stencil(������ ���� �� ���µ�)
	this->m_pDeviceContext->ClearDepthStencilView(this->m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Input Assembler�� ��ǲ ���̾ƿ��� �����Ѵ�.
	// �� �� ������Ƽ�꿡 ���� ������ �����Ѵ�. ���⼭�� �ﰢ���� �׸��� ���� ����Ʈ�� ������ �Ѵ�.
	this->m_pDeviceContext->IASetInputLayout(this->m_pVertexShader.GetInputLayout());
	this->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �����Ͷ����� ����(Rasterize State, RS)�� ����̽� ���ؽ�Ʈ�� �����ؾ��Ѵ�.
	this->m_pDeviceContext->RSSetState(m_pRasterizerState.Get());

	// OM�� Depth/Stencil State�� �����Ѵ�.
	this->m_pDeviceContext->OMSetDepthStencilState(this->m_pDepthStencilState.Get(), 0);

	// OM�� ������ �����Ѵ�.
	this->m_pDeviceContext->OMSetBlendState(this->m_pBlendState.Get(), NULL, 0xFFFFFFFF);

	// �ȼ� ���̴��� �ؽ�ó�� ���� ���÷� ���¿� ���� �ִ´�. (�̷��� �ϸ� ���̴��� �������Ϳ� �ִ´�.)
	this->m_pDeviceContext->PSSetSamplers(0, 1, this->m_pSamplerState.GetAddressOf());

	// ���̴��� ���� ������ �Ѵ�.
	this->m_pDeviceContext->VSSetShader(m_pVertexShader.GetShader(), NULL, 0);
	this->m_pDeviceContext->PSSetShader(m_pPixelShader.GetShader(), NULL, 0);

	// ���̴� ���۸� IA�� �������ش�.
	UINT offset = 0;

	// ��� ���۸� ������Ʈ�Ѵ�.
	// m_pConstBuffer.GetData().mat = DirectX::XMMatrixTranslation(0.0f, -0.5f, 0.0f);				// ������� �ϸ� column_major
	// m_pConstBuffer.GetData().mat = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	// m_pConstBuffer.GetData().mat = DirectX::XMMatrixTranspose(m_pConstBuffer.GetData().mat);	// ������� �ϸ� row_major

	{	// ����
		static float translationOffset[3] = { 0,0, 4.0 };

		DirectX::XMMATRIX worldMatrix = XMMatrixScaling(5.0f, 5.0f, 5.0f) *
			DirectX::XMMatrixTranslation(translationOffset[0],
				translationOffset[1],
				translationOffset[2]);

		// ���� ��� ����
		// view�� projection�� ī�޶� �������ش�.
		m_pConstBufferVertexShader.GetData().mat = worldMatrix *
			GET_SINGLE(CCamera)->GetViewMatrix() * GET_SINGLE(CCamera)->GetProjectionMatrix();
		m_pConstBufferVertexShader.GetData().mat = DirectX::XMMatrixTranspose(m_pConstBufferVertexShader.GetData().mat);
		if (!m_pConstBufferVertexShader.ApplyChanges())
		{
			return;
		}
		m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstBufferVertexShader.GetAddressOf());

		// �ȼ� ��� ����
		m_pConstBufferPixelShader.GetData().alpha = 1.0f;
		if (!m_pConstBufferPixelShader.ApplyChanges())
		{
			return;
		}
		m_pDeviceContext->PSSetConstantBuffers(0, 1, m_pConstBufferPixelShader.GetAddressOf());

		// �簢��
		this->m_pDeviceContext->PSSetShaderResources(0, 1, m_pPavementTexture.GetAddressOf());
		this->m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), m_pVertexBuffer.GetStridePtr(), &offset);
		// (�ε��� ���� �̿�)
		this->m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		// ���� ����� ������ ���� �����͸� �׸���.
		// �ε��� ���ۿ� �ִ� �����Ϳ� ���� �׸���.
		this->m_pDeviceContext->DrawIndexed(m_pIndexBuffer.GetBufferSize(), 0, 0);
	}

	static float alpha = 1.0f;
	{	// ��ũ
		static float translationOffset[3] = { 0,0, -1 };

		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(translationOffset[0],
			translationOffset[1],
			translationOffset[2]);

		// ���� ��� ����
		// view�� projection�� ī�޶� �������ش�.
		m_pConstBufferVertexShader.GetData().mat = worldMatrix *
			GET_SINGLE(CCamera)->GetViewMatrix() * GET_SINGLE(CCamera)->GetProjectionMatrix();
		m_pConstBufferVertexShader.GetData().mat = DirectX::XMMatrixTranspose(m_pConstBufferVertexShader.GetData().mat);
		if (!m_pConstBufferVertexShader.ApplyChanges())
		{
			return;
		}
		m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstBufferVertexShader.GetAddressOf());

		// �ȼ� ��� ����
		m_pConstBufferPixelShader.GetData().alpha = alpha;
		if (!m_pConstBufferPixelShader.ApplyChanges())
		{
			return;
		}
		m_pDeviceContext->PSSetConstantBuffers(0, 1, m_pConstBufferPixelShader.GetAddressOf());

		// �簢��
		this->m_pDeviceContext->PSSetShaderResources(0, 1, m_pPinkTexture.GetAddressOf());
		this->m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), m_pVertexBuffer.GetStridePtr(), &offset);
		// (�ε��� ���� �̿�)
		this->m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		// ���� ����� ������ ���� �����͸� �׸���.
		// �ε��� ���ۿ� �ִ� �����Ϳ� ���� �׸���.
		this->m_pDeviceContext->DrawIndexed(m_pIndexBuffer.GetBufferSize(), 0, 0);
	}


	{	// Ǯ
		static float translationOffset[3] = { 0,0,0 };

		DirectX::XMMATRIX worldMatrix = XMMatrixScaling(5.0f, 5.0f, 5.0f) *
			DirectX::XMMatrixTranslation(translationOffset[0],
				translationOffset[1],
				translationOffset[2]);

		// ���� ��� ����
		// view�� projection�� ī�޶� �������ش�.
		m_pConstBufferVertexShader.GetData().mat = worldMatrix *
			GET_SINGLE(CCamera)->GetViewMatrix() * GET_SINGLE(CCamera)->GetProjectionMatrix();
		m_pConstBufferVertexShader.GetData().mat = DirectX::XMMatrixTranspose(m_pConstBufferVertexShader.GetData().mat);
		if (!m_pConstBufferVertexShader.ApplyChanges())
		{
			return;
		}
		m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstBufferVertexShader.GetAddressOf());

		// �ȼ� ��� ����
		m_pConstBufferPixelShader.GetData().alpha = 1.0f;
		if (!m_pConstBufferPixelShader.ApplyChanges())
		{
			return;
		}
		m_pDeviceContext->PSSetConstantBuffers(0, 1, m_pConstBufferPixelShader.GetAddressOf());

		// �簢��
		this->m_pDeviceContext->PSSetShaderResources(0, 1, m_pGrassTexture.GetAddressOf());
		this->m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), m_pVertexBuffer.GetStridePtr(), &offset);
		// (�ε��� ���� �̿�)
		this->m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		// ���� ����� ������ ���� �����͸� �׸���.
		// �ε��� ���ۿ� �ִ� �����Ϳ� ���� �׸���.
		this->m_pDeviceContext->DrawIndexed(m_pIndexBuffer.GetBufferSize(), 0, 0);
	}

	// ��Ʈ�� �׸���.
	m_pSpriteBatch->Begin();
	m_pSpriteFont->DrawString(m_pSpriteBatch.get(), L"Hello World", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 
		0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	m_pSpriteBatch->End();

	// ImGui ������ ������
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	// ImGui �׽�Ʈ ������ ����
	ImGui::Begin("Test");

	ImGui::Text("This is an example Text.");
	//static int cnt = 0;
	// ��ư
	//if (ImGui::Button("Click this!"))
	//{
	//	cnt++;
	//}
	// ImGui::SameLine();
	//std::string clickCount = "Click Count : " + std::to_string(cnt);
	//ImGui::Text(clickCount.c_str());

	// ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -5.0f, 5.0f);
	ImGui::DragFloat("Alpha", &alpha, 0.1f, 0.0f, 1.0f);

	ImGui::End();

	// ���� ��� �������� ���ļ� �������Ѵ�.
	ImGui::Render();
	
	// �׸� �����͸� �������Ѵ�.
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// �׸��� �̸� ���� ü�ο� �����Ѵ�.
	this->m_pSwapChain->Present(1, NULL);
}
