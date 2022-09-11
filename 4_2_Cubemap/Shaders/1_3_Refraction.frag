#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;

uniform vec3 cameraPos;
uniform samplerCube skybox;

vec3 refraction(vec3 N, vec3 V, float r);

void main()
{
	float ratio = 1.00/1.52;
	vec3 V = normalize(cameraPos - Position);
	vec3 T =  refract(-V, normalize(Normal), ratio);
	//vec3 T =  refraction(normalize(Normal), V, ratio);
	
	if(T == vec3(0.0, -1.0, 0.0))
	{
		FragColor = vec4(1.0);
	}
	else
	{
		FragColor = vec4(texture(skybox, T).rgb, 1.0);
	}

}

vec3 refraction(vec3 N, vec3 V, float r)
{
	float cos_i = dot(N,V);
	float k = 1 - r*r*(1-cos_i*cos_i);
	
	if(k < 0.0)
		return vec3(0.0);
	else
	{
		float a = cos_i*r - sqrt(k);
		return normalize(-V*r + N*a);
	}
}