#include "CCore.h"
#include "Timer/CTimer.h"
#include "Input/CInput.h"
#include "Path/CPathManager.h"
#include "Camera/CCamera.h"

// nullptr 선언은 여기서 가능하다.
// 왜냐면 얘는 프로그램 시작과 생기는 인스턴스고
// 전역변수니까(static)
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

    // 윈도우 클래스 등록
    MyRegisterClass();

    // 해상도 설정
    m_tRS.nWidth = nWidth;
    m_tRS.nHeight = nHeight;

    // 윈도우창 생성
    Create();

    // 카메라 매니저 초기화(얘가 먼저 초기화가 안 되면, 그래픽에서 오류 발생)
    if (!GET_SINGLE(CCamera)->Init())
    {
        return false;
    }

    // 경로관리자 초기화
    if (!GET_SINGLE(CPathManager)->Init())
    {
        return false;
    }

    // 타이머(FPS, 델타타임) 초기화
    if (!GET_SINGLE(CTimer)->Init(m_hWnd))
    {
        return false;
    }

    // 인풋 매니저 초기화
    if (!GET_SINGLE(CInput)->Init(m_hWnd, false))
    {
        return false;
    }

    // 그래픽 객체 생성
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
        // 윈도우 메시지 큐를 처리하는 함수입니다.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 윈도우 데드타임
        else
        {
            // 게임의 로직이 실행된다.
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
        // 컨트롤러
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

    // 실제 타이틀바나 윈도우를 포함한 크기를 구해준다.
    RECT rc = { 0,0,m_tRS.nWidth,m_tRS.nHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // 위에서 구해준 크기로 윈도우 클라이언트 크기를 원하는 크기로 맞춰줘야 한다.
    SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left,
        rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return TRUE;
}

void CCore::Logic()
{ 
    // 타이머 갱신
    GET_SINGLE(CTimer)->Update();

    // 우리가 함수를 만들고 그 델타타임에 이것을 전달하면 된다.
    float fDeltaTime = GET_SINGLE(CTimer)->GetDeltaTime();

    // 입력을 받는다.
    Input(fDeltaTime);
    
    // if (Update(fDeltaTime) == SC_CHANGE)
    // {
    //     return; // 장면 변환이 일어난다면 
    // }
    // if (LateUpdate(fDeltaTime) == SC_CHANGE)
    // {
    //     return; // 장면 변환이 일어난다면 
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
    // 메모리 누수를 체크하기 위한 설정
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // 메모리 릭이 있을 때만 쓴다.
    // 매개변수로 그 메모리의 블록 번호를 넣어주면,
    // 메모리 릭 부분을 바로 이동한다. (호출 스택을 잘 볼 수 있어야..)
    // _CrtSetBreakAlloc(313);

    // 컴파일 시간에 체크해서 이걸 동작시킬지 말지 결정한다.
#ifdef _DEBUG   
    // 콘솔창을 생성시켜주는 함수
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

    // 콘솔창 해제
#ifdef _DEBUG
    FreeConsole();
#endif // _DEBUG
}
