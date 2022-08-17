#version 330 core
out vec4 fragColor;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Texture
{
	sampler2D texture_diffuse1;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform Material mat;

uniform Texture tex;

uniform vec3 viewPos;
uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	
	// Directional Light
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	fragColor = vec4(result, 1.0);

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir =	normalize(-light.direction);

	// Ambient
	vec3 tex_ambient = vec3(texture(tex.texture_diffuse1, texCoord));
	vec3 ambient = vec3(1.0);
	if(tex_ambient == vec3(0.0))
	{
		ambient = light.ambient * mat.ambient;
	}
	else
	{
		ambient = light.ambient * tex_ambient * mat.ambient;
	}

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 tex_diffuse = vec3(texture(tex.texture_diffuse1, texCoord));
	vec3 diffuse = vec3(1.0);
	if(tex_diffuse == vec3(0.0))
	{
		diffuse = light.diffuse * diff * mat.diffuse;
	}
	else
	{
		diffuse = light.diffuse * diff * tex_diffuse * mat.diffuse;
	}

	// Specular
	// vec3 reflectDir = 2 * dot(normal, lightDir) * normal - lightDir;
	// float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);	
	// vec3 specular = light.specular * specFactor * vec3(texture(material.specular, texCoord));

	return (ambient + diffuse);
}