#pragma once

#include <string>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Light
{
public:
	Light();
	Light(vec3 dir, vec3 amb, vec3 diff, vec3 spec);

	// Getter	
	vec3 GetDir() { return direction; };
	vec3 GetAmb() { return ambient; };
	vec3 GetDiff() { return diffuse; };
	vec3 GetSpec() { return specular; };

	// Setter
	void SetDir(vec3 dir) { direction = dir; };

	~Light();

private:
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};