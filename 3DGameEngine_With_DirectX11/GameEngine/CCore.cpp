#include "CCore.h"

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

bool CCore::Init(HINSTANCE hInstance)
{
    m_hInst = hInstance;

    // ������ Ŭ���� ���
    MyRegisterClass();

    // �ػ� ����
    m_tRS.nWidth = 1280;
    m_tRS.nHeight = 720;

    // ������â ����
    Create();

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
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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
    wcex.lpszClassName = L"FroK's Engine";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

BOOL CCore::Create()
{
    m_hWnd = CreateWindowW(
        L"FroK's Engine",
        L"FroK's Engine",
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
    // GET_SINGLE(CTimer)->Update();

    // �츮�� �Լ��� ����� �� ��ŸŸ�ӿ� �̰��� �����ϸ� �ȴ�.
    // float fDeltaTime = GET_SINGLE(CTimer)->GetDeltaTime();

    // �Է��� �޴´�.
    // Input(fDeltaTime);
    // 
    // if (Update(fDeltaTime) == SC_CHANGE)
    // {
    //     return; // ��� ��ȯ�� �Ͼ�ٸ� 
    // }
    // if (LateUpdate(fDeltaTime) == SC_CHANGE)
    // {
    //     return; // ��� ��ȯ�� �Ͼ�ٸ� 
    // }
    // 
    // Collision(fDeltaTime);
    // Render(fDeltaTime);
}

void CCore::Input(float fDeltaTime)
{
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
}

CCore::CCore()
{ 
    // �޸� ������ üũ�ϱ� ���� ����
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // �޸� ���� ���� ���� ����.
    // �Ű������� �� �޸��� ���� ��ȣ�� �־��ָ�,
    // �޸� �� �κ��� �ٷ� �̵��Ѵ�. (ȣ�� ������ �� �� �� �־��..)
    // _CrtSetBreakAlloc(313);

    // ������ �ð��� üũ�ؼ� �̰� ���۽�ų�� ���� �����Ѵ�.
#ifdef _DEBUG   
    // �ܼ�â�� ���������ִ� �Լ�
    AllocConsole();
#endif
}

CCore::~CCore()
{

    // �ܼ�â ����
#ifdef _DEBUG
    FreeConsole();
#endif // _DEBUG
}