#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct EUTS_RenderState;

void EUTS_DebugRender_initialize(EUTS_RenderState *renderState);

void EUTS_DebugRender_finalize();

void EUTS_DebugRender_drawLine(XMFLOAT3 p1, XMFLOAT3 p2, EUTS_RenderState *renderState);