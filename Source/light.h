#pragma once
#include "render.h"


struct EUTS_SunLight
{
	XMFLOAT4 direction;
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
};

void EUTS_SunLight_setDirection(EUTS_SunLight *light, float x, float y, float z);

void EUTS_SunLight_setDiffuse(EUTS_SunLight *light, float r, float g, float b);

void EUTS_SunLight_setAmbient(EUTS_SunLight *light, float r, float g, float b);

