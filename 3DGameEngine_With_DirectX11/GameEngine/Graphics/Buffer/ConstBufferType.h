#pragma once

#include "../../Game.h"

typedef struct _CB_VS_VERTEXSHADER
{
	DirectX::XMMATRIX mat;
}CB_VS_VERTEXSHADER, * PCB_VS_VERTEXSHADER;

typedef struct _CB_PS_PIXELSHADER
{
	float alpha = 1.0f;
}CB_PS_PIXELSHADER, *PCB_PS_PIXELSHADER;