#include "light.h"

void EUTS_SunLight_setDirection(EUTS_SunLight *light, float x, float y, float z)
{
	light->direction.x = x;
	light->direction.y = y;
	light->direction.z = z;
	light->direction.w = 1.0f;
}

void EUTS_SunLight_setDiffuse(EUTS_SunLight *light, float r, float g, float b)
{
	light->diffuse.x = r;
	light->diffuse.y = g;
	light->diffuse.z = b;
	light->diffuse.w = 1.0f;
}

void EUTS_SunLight_setAmbient(EUTS_SunLight *light, float r, float g, float b)
{
	light->ambient.x = r;
	light->ambient.y = g;
	light->ambient.z = b;
	light->ambient.w = 1.0f;
}