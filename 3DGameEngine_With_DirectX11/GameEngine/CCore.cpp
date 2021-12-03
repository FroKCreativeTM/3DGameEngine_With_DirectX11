#include "CCore.h"
#include "Timer/CTimer.h"
#include "Input/CInput.h"
#include "Path/CPathManager.h"
#include "Camera/CCamera.h"

// nullptr ������ ���⼭ �����ϴ�.
// �ֳĸ� ��� ���α׷� ���۰� ����� �ν��Ͻ���
// ���������ϱ�(static)
CCore* CCore::m_pInstance = nullptr;
bool   CCore::m_bLoop = true;

CCore* CCore::GetInst()
{
	if (!m_pInstance)
	{
		m_pInstance = new CCore;
	}
	return m_pInstance;
}

void CCore::DestroyInst()
{
	SAFE_DELETE(m_pInstance);
}

bool CCore::Init(HINSTANCE hInstance,
    int nWidth, int nHeight)
{
    m_hInst = hInstance;

    // ������ Ŭ���� ���
    MyRegisterClass();

    // �ػ� ����
    m_tRS.nWidth = nWidth;
    m_tRS.nHeight = nHeight;

    // ������â ����
    Create();

    // ī�޶� �Ŵ��� �ʱ�ȭ(�갡 ���� �ʱ�ȭ�� �� �Ǹ�, �׷��ȿ��� ���� �߻�)
    if (!GET_SINGLE(CCamera)->Init())
    {
        return false;
    }

    // ��ΰ����� �ʱ�ȭ
    if (!GET_SINGLE(CPathManager)->Init())
    {
        return false;
    }

    // Ÿ�̸�(FPS, ��ŸŸ��) �ʱ�ȭ
    if (!GET_SINGLE(CTimer)->Init(m_hWnd))
    {
        return false;
    }

    // ��ǲ �Ŵ��� �ʱ�ȭ
    if (!GET_SINGLE(CInput)->Init(m_hWnd, false))
    {
        return false;
    }

    // �׷��� ��ü ����
    if (!m_graphics.Initialize(m_hWnd, m_tRS.nWidth, m_tRS.nHeight))
    {
        return false;
    }

    return true;
}

int CCore::Run()
{
    MSG msg;
    ::ZeroMemory(&msg, sizeof(MSG));

    while (m_bLoop)
    {
        // ������ �޽��� ť�� ó���ϴ� �Լ��Դϴ�.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // ������ ����Ÿ��
        else
        {
            // ������ ������ ����ȴ�.
            Logic();
        }
    }

    return (int)msg.wParam;
}

LRESULT CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        m_bLoop = false;
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN: case WM_SYSKEYDOWN:
        GET_SINGLE(CInput)->KeyDown(wParam);
        return 0;
    case WM_KEYUP: case WM_SYSKEYUP:
        GET_SINGLE(CInput)->KeyUp(wParam);
        return 0;
    case WM_CHAR:
        GET_SINGLE(CInput)->KeyIn(wParam);
        return 0;
    case WM_MOUSEMOVE:
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_INPUT:
        GET_SINGLE(CInput)->MouseRawIn(lParam);
        return 0;
    case WM_LBUTTONDOWN:
        GET_SINGLE(CInput)->SetMouseLButton(true);
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_LBUTTONUP:
        GET_SINGLE(CInput)->SetMouseLButton(false);
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_MBUTTONDOWN:
        GET_SINGLE(CInput)->SetMouseMButton(true);
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_MBUTTONUP:
        GET_SINGLE(CInput)->SetMouseMButton(false);
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_RBUTTONDOWN:
        GET_SINGLE(CInput)->SetMouseRButton(true);
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_RBUTTONUP:
        GET_SINGLE(CInput)->SetMouseRButton(false);
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_XBUTTONDOWN: case WM_XBUTTONUP:
        GET_SINGLE(CInput)->SetMouseXButton(wParam);
        GET_SINGLE(CInput)->MouseIn(lParam);
        return 0;
    case WM_DEVICECHANGE:
        // ��Ʈ�ѷ�
        GET_SINGLE(CInput)->CheckControllers();
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM CCore::MyRegisterClass()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = CCore::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInst;
    wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"FroK's DirectX 3D Engine";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

BOOL CCore::Create()
{
    m_hWnd = CreateWindowW(
        L"FroK's DirectX 3D Engine",
        L"FroK's DirectX 3D Engine",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
        nullptr, nullptr, m_hInst, nullptr);

    if (!m_hWnd)
    {
        return FALSE;
    }

    // ���� Ÿ��Ʋ�ٳ� �����츦 ������ ũ�⸦ �����ش�.
    RECT rc = { 0,0,m_tRS.nWidth,m_tRS.nHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // ������ ������ ũ��� ������ Ŭ���̾�Ʈ ũ�⸦ ���ϴ� ũ��� ������� �Ѵ�.
    SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left,
        rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return TRUE;
}

void CCore::Logic()
{ 
    // Ÿ�̸� ����
    GET_SINGLE(CTimer)->Update();

    // �츮�� �Լ��� ����� �� ��ŸŸ�ӿ� �̰��� �����ϸ� �ȴ�.
    float fDeltaTime = GET_SINGLE(CTimer)->GetDeltaTime();

    // �Է��� �޴´�.
    Input(fDeltaTime);
    
    // if (Update(fDeltaTime) == SC_CHANGE)
    // {
    //     return; // ��� ��ȯ�� �Ͼ�ٸ� 
    // }
    // if (LateUpdate(fDeltaTime) == SC_CHANGE)
    // {
    //     return; // ��� ��ȯ�� �Ͼ�ٸ� 
    // }
    
    Collision(fDeltaTime);

    Render(fDeltaTime);
}

void CCore::Input(float fDeltaTime)
{
    if (GET_SINGLE(CInput)->GetMouseRButton())
    {
        GET_SINGLE(CCamera)->AdjustRotation((float)GET_SINGLE(CInput)->GetMouseRawY() * 0.01f,
            (float)GET_SINGLE(CInput)->GetMouseRawX() * 0.01f, 0);
    }
    const float CAMERA_SPEED = 0.02f;

    if (GET_SINGLE(CInput)->IsKeyDown('W'))
    {
        GET_SINGLE(CCamera)->AdjustPosition(GET_SINGLE(CCamera)->GetForwardVector() * CAMERA_SPEED);
    }
    if (GET_SINGLE(CInput)->IsKeyDown('S'))
    {
        GET_SINGLE(CCamera)->AdjustPosition(GET_SINGLE(CCamera)->GetBackwardVector() * CAMERA_SPEED);
    }
    if (GET_SINGLE(CInput)->IsKeyDown('A'))
    {
        GET_SINGLE(CCamera)->AdjustPosition(GET_SINGLE(CCamera)->GetLeftVector() * CAMERA_SPEED);
    }
    if (GET_SINGLE(CInput)->IsKeyDown('D'))
    {
        GET_SINGLE(CCamera)->AdjustPosition(GET_SINGLE(CCamera)->GetRightVector() * CAMERA_SPEED);
    }
    if (GET_SINGLE(CInput)->IsKeyDown(VK_SPACE))
    {
        GET_SINGLE(CCamera)->AdjustPosition(0.0f, CAMERA_SPEED, 0.0f);
    }
    if (GET_SINGLE(CInput)->IsKeyDown('Z'))
    {
        GET_SINGLE(CCamera)->AdjustPosition(0.0f, CAMERA_SPEED, 0.0f);
    }
    if (GET_SINGLE(CInput)->IsKeyDown(VK_ESCAPE))
    {
        m_bLoop = false;
        PostQuitMessage(0);
    }
}

int CCore::Update(float fDeltaTime)
{
	return 0;
}

int CCore::LateUpdate(float fDeltaTime)
{
	return 0;
}

void CCore::Collision(float fDeltaTime)
{
}

void CCore::Render(float fDeltaTime)
{
    m_graphics.Render(fDeltaTime);
}

CCore::CCore()
{ 
    // �޸� ������ üũ�ϱ� ���� ����
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // �޸� ���� ���� ���� ����.
    // �Ű������� �� �޸��� ��� ��ȣ�� �־��ָ�,
    // �޸� �� �κ��� �ٷ� �̵��Ѵ�. (ȣ�� ������ �� �� �� �־��..)
    // _CrtSetBreakAlloc(313);

    // ������ �ð��� üũ�ؼ� �̰� ���۽�ų�� ���� �����Ѵ�.
#ifdef _DEBUG   
    // �ܼ�â�� ���������ִ� �Լ�
    if (AllocConsole())
    {
        FILE* nfp[3];
        freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
        freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
        freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
        std::ios::sync_with_stdio();
    }
#endif
}

CCore::~CCore()
{
    DESTROY_SINGLE(CInput);
    DESTROY_SINGLE(CTimer);
    DESTROY_SINGLE(CPathManager);
    DESTROY_SINGLE(CCamera);

    // �ܼ�â ����
#ifdef _DEBUG
    FreeConsole();
#endif // _DEBUG
}
