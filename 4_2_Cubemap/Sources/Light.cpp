#include "Light.h"

Light::Light()
{
}

Light::Light(vec3 amb, vec3 diff, vec3 spec, vec3 dir)
{	
	direction = dir;
	ambient = amb;
	diffuse = diff;
	specular = spec;
}

Light::~Light()
{
}


