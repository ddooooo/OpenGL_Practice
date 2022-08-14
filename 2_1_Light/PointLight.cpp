#include "PointLight.h"

PointLight::PointLight()
{
}

PointLight::PointLight(vec3 amb, vec3 diff, vec3 spec,
					   vec3 pos, float constant, float linear, float quadratic,
					   vec3 dir) : Light(amb, diff, spec, dir)
{
	position = pos;

	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

PointLight::~PointLight()
{
}


