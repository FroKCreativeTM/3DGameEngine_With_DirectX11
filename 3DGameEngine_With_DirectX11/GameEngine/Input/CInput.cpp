#include "CInput.h"

CInput::CInput() {
	for (size_t i = 0; i < NSInput::KEYS_ARRAY_LEN; i++) {
		m_keysDown[i] = false;
	}

	for (size_t i = 0; i < NSInput::KEYS_ARRAY_LEN; i++) {
		m_keysPressed[i] = false;
	}

	m_newLine = true;
	m_textIn = "";
	m_charIn = 0;
	m_mouseX = 0;
	m_mouseY = 0;
	m_mouseRawX = 0;
	m_mouseRawY = 0;
	m_mouseLButton = false;
	m_mouseMButton = false;
	m_mouseRButton = false;
	m_mouseX1Button = false;
	m_mouseX2Button = false;

	for (size_t i = 0; i < MAX_CONTROLLERS; i++) {
		m_controllers[i].vibrateTimeLeft = 0;
		m_controllers[i].vibrateTimeRight = 0;
	}
}

CInput::~CInput() {
	if (m_mouseCaptured) {
		ReleaseCapture();
	}
}

void CInput::Initialize(HWND hwnd, bool capture) {
	try {
		m_mouseCaptured = capture;

		m_Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		m_Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		m_Rid[0].dwFlags = RIDEV_INPUTSINK;
		m_Rid[0].hwndTarget = hwnd;
		RegisterRawInputDevices(m_Rid, 1, sizeof(m_Rid[0]));

		if (m_mouseCaptured) {
			SetCapture(hwnd);
		}

		// 컨트롤러 상태를 지운다.
		ZeroMemory(m_controllers, sizeof(ControllerState) * MAX_CONTROLLERS);
		CheckControllers();
	}
	catch (...) {
		throw(CGameError(NSGameError::FATAL_ERROR,
			"Error initializing input system."));
	}
}

void CInput::KeyDown(WPARAM wParam) {
	// 범위 확인을 한다.
	if (wParam < NSInput::KEYS_ARRAY_LEN) {
		m_keysDown[wParam] = true;		// KeysDown 배열 갱신
		m_keysPressed[wParam] = true;	// KeysPressed 배열 갱신
	}
}

void CInput::KeyUp(WPARAM wParam) {
	if (wParam < NSInput::KEYS_ARRAY_LEN) {
		m_keysDown[wParam] = false;		// KeysDown 배열 갱신
	}
}

void CInput::KeyIn(WPARAM wParam) {
	if (m_newLine) {
		m_textIn.clear();
		m_newLine = false;
	}
	if (wParam == '\b') { // 백스페이스인 경우
		if (m_textIn.length() > 0) { // 문자가 존재한다면
			m_textIn.erase(m_textIn.size() - 1); // 마지막에 입력한 문자를 지운다.
		}
	}
	else {
		m_textIn += wParam;
		m_charIn = wParam;
	}

	if ((char)wParam == '\r') {
		m_newLine = true;
	}
}

bool CInput::IsKeyDown(UCHAR vkey) const {
	if (vkey < NSInput::KEYS_ARRAY_LEN) {
		return m_keysDown[vkey];
	}
	else {
		return false;
	}
}

bool CInput::WasKeyPressed(UCHAR vkey) const {
	if (vkey < NSInput::KEYS_ARRAY_LEN) {
		return m_keysPressed[vkey];
	}
	else {
		return false;
	}
}

bool CInput::AnyKeyPressed() const {
	for (size_t i = 0; i < NSInput::KEYS_ARRAY_LEN; i++)
	{
		if (m_keysPressed[i] == true) {
			return true;
		}
	}
	return false;
}

void CInput::ClearKeyPress(UCHAR vkey) {
	if (vkey < NSInput::KEYS_ARRAY_LEN) {
		m_keysPressed[vkey] = false;
	}
}

void CInput::Clear(UCHAR what) {
	if (what & NSInput::KEYS_DOWN) { // KeysDown을 지우고 싶다면
		for (size_t i = 0; i < NSInput::KEYS_ARRAY_LEN; i++) {
			m_keysDown[i] = false;
		}
	}
	if (what & NSInput::KEYS_PRESSED) { // KeysPressed을 지우고 싶다면
		for (size_t i = 0; i < NSInput::KEYS_ARRAY_LEN; i++) {
			m_keysPressed[i] = false;
		}
	}
	if (what & NSInput::MOUSE) {
		m_mouseX = 0;
		m_mouseY = 0;
		m_mouseRawX = 0;
		m_mouseRawY = 0;
	}
	if (what & NSInput::TEXT_IN) {
		ClearTextIn();
	}
}

void CInput::MouseIn(LPARAM lParam) {
	m_mouseX = GET_X_LPARAM(lParam);
	m_mouseY = GET_Y_LPARAM(lParam);
}

void CInput::MouseRawIn(LPARAM lParam) {
	UINT dwSize = 40;
	static BYTE lpb[40];

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEMOUSE) {
		m_mouseRawX = raw->data.mouse.lLastX;
		m_mouseRawY = raw->data.mouse.lLastY;
	}
}

void CInput::MouseWheelIn(WPARAM wParam) {
	m_mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
}

void CInput::CheckControllers() {
	DWORD result;
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		result = XInputGetState(i, &m_controllers[i].state);
		if (result == ERROR_SUCCESS)
			m_controllers[i].connected = true;
		else
			m_controllers[i].connected = false;
	}
}

void CInput::ReadControllers() {
	DWORD result;
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (m_controllers[i].connected)
		{
			result = XInputGetState(i, &m_controllers[i].state);
			if (result == ERROR_DEVICE_NOT_CONNECTED)    // 컨트롤러가 접속이 끊겼다면
				m_controllers[i].connected = false;
		}
	}
}

BYTE CInput::GetGamepadLeftTrigger(UINT n) {
	BYTE value = GetGamepadLeftTriggerUndead(n);
	if (value > triggerDeadzone)
		value = (value - triggerDeadzone) * 255 /
		(255 - triggerDeadzone);
	else                                   
		value = 0;
	return value;
}

BYTE CInput::GetGamepadRightTrigger(UINT n) {
	BYTE value = GetGamepadRightTriggerUndead(n);
	if (value > triggerDeadzone)    
		value = (value - triggerDeadzone) * 255 /
		(255 - triggerDeadzone);
	else                                    
		value = 0;
	return value;
}

SHORT CInput::GetGamepadThumbLX(UINT n) {
	int x = GetGamepadThumbLXUndead(n);
	if (x > thumbstickDeadzone)
		x = (x - thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else if (x < -thumbstickDeadzone) 
		x = (x + thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else        
		x = 0;  
	return static_cast<SHORT>(x);
}

SHORT CInput::GetGamepadThumbLY(UINT n) {
	int y = GetGamepadThumbLYUndead(n);
	if (y > thumbstickDeadzone) 
		y = (y - thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else if (y < -thumbstickDeadzone) 
		y = (y + thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else        
		y = 0;  
	return static_cast<SHORT>(y);
}

SHORT CInput::GetGamepadThumbRX(UINT n) {
	int x = GetGamepadThumbRXUndead(n);
	if (x > thumbstickDeadzone) 
		x = (x - thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else if (x < -thumbstickDeadzone) 
		x = (x + thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else       
		x = 0; 
	return static_cast<SHORT>(x);
}

SHORT CInput::GetGamepadThumbRY(UINT n) {
	int y = GetGamepadThumbRYUndead(n);
	if (y > thumbstickDeadzone)
		y = (y - thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else if (y < -thumbstickDeadzone)  
		y = (y + thumbstickDeadzone) * 32767 /
		(32767 - thumbstickDeadzone);
	else        
		y = 0;  
	return static_cast<SHORT>(y);
}

void CInput::VibrateControllers(float frameTime) {
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (m_controllers[i].connected)
		{
			m_controllers[i].vibrateTimeLeft -= frameTime;
			if (m_controllers[i].vibrateTimeLeft < 0)
			{
				m_controllers[i].vibrateTimeLeft = 0;
				m_controllers[i].vibration.wLeftMotorSpeed = 0;
			}
			m_controllers[i].vibrateTimeRight -= frameTime;
			if (m_controllers[i].vibrateTimeRight < 0)
			{
				m_controllers[i].vibrateTimeRight = 0;
				m_controllers[i].vibration.wRightMotorSpeed = 0;
			}
			XInputSetState(i, &m_controllers[i].vibration);
		}
	}
}
