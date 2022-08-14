#pragma once

#include "PointLight.h"

class SpotLight : public PointLight
{
public:
	SpotLight();

	SpotLight(vec3 amb, vec3 diff, vec3 spec,
		      float constant, float linear, float quadratic, 
		      float inner, float outer, 
		      vec3 pos = { 0.0f, 0.0f, 0.0f }, vec3 dir = { 0.0f, 0.0f, 0.0f });
	~SpotLight();

	// Getter
	float GetCutOff() { return cutOff; };
	float GetOuterCutOff() { return outerCutOff; };

private:
	float cutOff;
	float outerCutOff;

};