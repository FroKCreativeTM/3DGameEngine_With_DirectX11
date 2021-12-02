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

	HRESULT hr = D3D11CreateDeviceAndSwapChain(	adapters[0].m_pAdapter,					// 연결될 IDXGI Adapter
												D3D_DRIVER_TYPE_UNKNOWN,				// 딱히 지정되지 않은 드라이버 타입
												NULL,									// 소프트웨어 드라이버 타입
												NULL,									// 런타임 레이어를 위한 플래그
												NULL,									// 피처 레벨 배열을 위한 
												0,										// 배열 속의 피처 레벨의 #
												D3D11_SDK_VERSION,
												&scd,									// 스왑 체인 디스크립션(설명서 구조체라 봐도 된다)
												this->m_pSwapChain.GetAddressOf(),		// 스왑체인 관리자 주소
												this->m_pDevice.GetAddressOf(),			// 디바이스 주소
												NULL,									// 지원되는 피처레벨
												this->m_pDeviceContext.GetAddressOf());	// 디바이스 컨텍스트 주소

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

	// Depth/Stencil 버퍼에 대한 설정
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

	// Output Merger를 담당
	this->m_pDeviceContext->OMSetRenderTargets(
		1,
		this->m_pRenderingTargetView.GetAddressOf(),
		this->m_pDepthStencilView.Get());

	// Depth Stencil state를 생성한다.
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	// 만약 같은 Depth를 가지고 있다면 나중에 들어온 것을 띄운다.

	hr = this->m_pDevice->CreateDepthStencilState(&depthStencilStateDesc,
		this->m_pDepthStencilState.GetAddressOf());

	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initD3DApp() CreateDepthStencilState()"));
		return false;
	}

	// 이제 뷰포트에 대한 설정을 한다.
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	// 뷰포트 크기 설정
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_nWindowWidth;
	viewport.Height = m_nWindowHeight;

	// 뷰포트의 깊이 설정
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// 뷰포트를 적용한다.
	this->m_pDeviceContext->RSSetViewports(1, &viewport);
	 
	// 레스터라이저에 대한 디스크립션을 설정하고 이를 레스터라이저에 적용한다.
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

	// 폰트에 대한 생성
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
		// 1. sementic 이름 : 셰이더의 입력 시그네처 속의 이 요소와 관련된 HLSL의 이름
		// 2. sement idx 
		// 3. DXGI_FORMAT : 색상 타입 등등
		// 4. InputSlot
		// 5. AlignedByteOffset : 말 그대로 오프셋
		// 6. InputSlotClass : 
		// 7. InstanceDataStepRate

		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
		D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements = ARRAYSIZE(layout);

	// 정점 셰이더 실패시 false, 아니면 true
	if (!m_pVertexShader.Initialize(m_pDevice, L"..\\Bin\\VertexShader.cso",
		layout, numElements))
	{
		return false;
	}

	// 픽셀 셰이더 실패시 false, 아니면 true
	if (!m_pPixelShader.Initialize(m_pDevice, L"..\\Bin\\PixelShader.cso"))
	{
		return false;
	}

	return true;
}

bool CGraphics::initializeScene()
{
	// Z 버퍼를 위한 버텍스 1
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

	// 버텍스 버퍼를 생성한다.
	HRESULT hr = this->m_pVertexBuffer.Initialize(this->m_pDevice.Get(), vertices, ARRAYSIZE(vertices));
	if (FAILED(hr))
	{
		throw(CGameError(NSGameError::FATAL_ERROR, "Error CGraphics::initializeScene() m_pDevice->CreateBuffer() - Vertex"));
		return false;
	}

	// 인덱스 버퍼를 생성한다.
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

	// 상수 버퍼를 초기화한다
	hr = m_pConstBuffer.Initialize(this->m_pDevice.Get(), this->m_pDeviceContext.Get());
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

	return true;
}

void CGraphics::Render(float fDeltaTime)
{
	float bgColor[] = { 0.0f, 0.0f, 0.0f ,1.0f};
	// Render Targer View와 Depth/Stencil View를 비운다.
	this->m_pDeviceContext->ClearRenderTargetView(this->m_pRenderingTargetView.Get(), bgColor);
	// 3번째의 1.0f는 MaxDepth
	// 4번째는 Stencil(지금은 딱히 안 쓰는듯)
	this->m_pDeviceContext->ClearDepthStencilView(this->m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Input Assembler에 인풋 레이아웃을 설정한다.
	// 그 뒤 프리미티브에 대한 설정을 진행한다. 여기서는 삼각형을 그리기 위한 리스트로 설정을 한다.
	this->m_pDeviceContext->IASetInputLayout(this->m_pVertexShader.GetInputLayout());
	this->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 레스터라이즈 상태(Rasterize State, RS)를 디바이스 컨텍스트에 적용해야한다.
	this->m_pDeviceContext->RSSetState(m_pRasterizerState.Get());

	// OM에 Depth/Stencil State를 적용한다.
	this->m_pDeviceContext->OMSetDepthStencilState(this->m_pDepthStencilState.Get(), 0);

	// 픽셀 쉐이더에 텍스처를 위한 샘플러 상태에 대해 넣는다. (이렇게 하면 쉐이더에 레지스터에 넣는다.)
	this->m_pDeviceContext->PSSetSamplers(0, 1, this->m_pSamplerState.GetAddressOf());

	// 쉐이더에 대한 설정을 한다.
	this->m_pDeviceContext->VSSetShader(m_pVertexShader.GetShader(), NULL, 0);
	this->m_pDeviceContext->PSSetShader(m_pPixelShader.GetShader(), NULL, 0);

	// 쉐이더 버퍼를 IA에 적용해준다.
	UINT offset = 0;

	// 상수 버퍼를 업데이트한다.
	// m_pConstBuffer.GetData().mat = DirectX::XMMatrixTranslation(0.0f, -0.5f, 0.0f);				// 여기까지 하면 column_major
	// m_pConstBuffer.GetData().mat = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	// m_pConstBuffer.GetData().mat = DirectX::XMMatrixTranspose(m_pConstBuffer.GetData().mat);	// 여기까지 하면 row_major

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	// view와 projection은 카메라가 설정해준다.
	m_pConstBuffer.GetData().mat = worldMatrix *
		GET_SINGLE(CCamera)->GetViewMatrix() * GET_SINGLE(CCamera)->GetProjectionMatrix();
	m_pConstBuffer.GetData().mat = DirectX::XMMatrixTranspose(m_pConstBuffer.GetData().mat);
	if (!m_pConstBuffer.ApplyChanges())
	{
		return;
	}
	m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstBuffer.GetAddressOf());

	// 사각형
	this->m_pDeviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());
	this->m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), m_pVertexBuffer.GetStridePtr(), &offset);
	// (인덱스 버퍼 이용)
	this->m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	// 다음 적용된 설정에 따라서 데이터를 그린다.
	// 인덱스 버퍼에 있는 데이터에 맞춰 그린다.
	this->m_pDeviceContext->DrawIndexed(m_pIndexBuffer.GetBufferSize(), 0, 0);

	// 폰트를 그린다.
	m_pSpriteBatch->Begin();
	m_pSpriteFont->DrawString(m_pSpriteBatch.get(), L"Hello World", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 
		0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	m_pSpriteBatch->End();

	// 그리고 이를 스왑 체인에 적용한다.
	this->m_pSwapChain->Present(1, NULL);
}
