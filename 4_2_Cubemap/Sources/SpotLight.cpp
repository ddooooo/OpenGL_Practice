#include "SpotLight.h"

SpotLight::SpotLight()
{
}

SpotLight::SpotLight(vec3 amb, vec3 diff, vec3 spec, 
					 float constant, float linear, float quadratic, 
					 float inner, float outer, 
					 vec3 pos, vec3 dir)
					 : PointLight(amb, diff, spec, pos, constant, linear, quadratic, dir)
{
	this->cutOff = inner;
	this->outerCutOff = outer;
}

SpotLight::~SpotLight()
{
}


