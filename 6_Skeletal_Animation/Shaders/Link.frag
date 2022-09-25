#version 330 core
out vec4 fragColor;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
};

struct Texture
{
	sampler2D texture_diffuse1;
	sampler2D texture_reflection1;
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
flat in ivec4 boneIDs;
in vec4 weights;

uniform Material mat;

uniform Texture tex;

uniform vec3 viewPos;
uniform DirLight dirLight;

uniform int displayBoneIndex;

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
	vec3 reflectDir = 2 * dot(normal, lightDir) * normal - lightDir;
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);	
	vec3 specular = light.specular * specFactor * mat.specular;

	return (ambient + diffuse + specular);
}

void main()
{
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	
	// Directional Light
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	bool found = false;

	for(int i=0; i<4; ++i)
	{
		if(boneIDs[i] == displayBoneIndex && displayBoneIndex >= 0)
		{
			if(weights[i] >= 0.7)
			{
				fragColor = vec4(1.0, 0.0, 0.0, 0.0) * weights[i];
			}
			else if(weights[i] >= 0.4 && weights[i] <= 0.6)
			{
				fragColor = vec4(0.0, 1.0, 0.0, 0.0) * weights[i];
			}
			else if(weights[i] >= 0.1)
			{
				fragColor = vec4(0.0, 0.0, 1.0, 0.0) * weights[i];
			}

			found = true;
			break;
		}
	}

	if(!found)
	{
		vec3 diffuse = vec3(texture(tex.texture_diffuse1, texCoord));
		fragColor = vec4(diffuse, 1.0);
	}
}