#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();

	PointLight(vec3 amb, vec3 diff, vec3 spec, vec3 pos,
		       float constant, float linear, float quadratic, 
		       vec3 dir = {0.0f, 0.0f, 0.0f});
	~PointLight();


	// Getter
	vec3 GetPos() { return position; };

	float GetConst() { return constant; };
	float GetLinear() { return linear; };
	float GetQuad() { return quadratic; };

	// Setter
	void SetPos(vec3 pos) { position = pos; };

private:
	vec3 position;

	float constant;
	float linear;
	float quadratic;
};
