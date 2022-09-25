#include "PointLight.h"

PointLight::PointLight() : Light(vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f)),
					position(vec3(0.0f)), constant(0.0f), linear(0.0f), quadratic(0.0f)
{}

PointLight::PointLight(vec3 amb, vec3 diff, vec3 spec,
					   vec3 pos, float constant, float linear, float quadratic, vec3 dir) 
					: Light(amb, diff, spec, dir),
					  position(pos), constant(constant), linear(linear), quadratic(quadratic)
{}

PointLight::~PointLight()
{}


