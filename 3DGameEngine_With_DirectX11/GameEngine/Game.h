#pragma once

// ������ �Ѱ� �� ����

#define WIN32_LEAN_AND_MEAN

// C/C++ runtime header
#include <Windows.h>
#include <wrl/client.h>		//  COM ��ü
#include <comdef.h>			// _com_error
#include <list>
#include <vector>
#include <unordered_map>
#include <crtdbg.h>			// �޸� ���� üũ�� ���
#include <typeinfo>
#include <string>
#include <functional>
#include <conio.h>
#include <iostream>

// DirectX ���� ���̺귯�� �� ���
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <WICTextureLoader.h>		// for texture(DTK)
using namespace DirectX;

// ��Ʈ ���� ���
#include <SpriteBatch.h>
#include <SpriteFont.h>

using namespace std;

// custom header
#include "resource.h"
#include "Macro.h"
#include "Flag.h"
#include "Types.h"

// ���� ���꿡 �ʿ��� �����
#define PI		3.141592f
#define GRAVITY 9.8f

// ��� ���� �����
#define ROOT_PATH		"RootPath"
#define TEXTURE_PATH	"TexturePath"
#define SHADER_PATH		"ShaderPath"
#define DATA_PATH		"DataPath"
#define SOUND_PATH		"SoundPath"

// �ʿ��� ���� ���̺귯�� ��ũ
#pragma comment(lib, "msimg32") // �̹��� ���� ���


// STL�� ������ ���� �Լ�
// Ÿ���� ����� �ʿ䰡 ���� ������, ���뼺�� ����.
template <typename T>
void Safe_Delete_VecList(T& ref)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = ref.end();

	for (iter = ref.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	ref.clear();
}

// ���̾� �Ҹ��ڿ���
// ��� ������Ʈ�� �����.
template <typename T>
void Safe_Release_VecList(T& ref)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = ref.end();

	for (iter = ref.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE((*iter));
	}

	ref.clear();
}

template <typename T>
void Safe_Delete_Map(T& ref)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = ref.end();

	for (iter = ref.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	ref.clear();
}

template <typename T>
void Safe_Release_Map(T& ref)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = ref.end();

	for (iter = ref.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second);
	}

	ref.clear();
}
