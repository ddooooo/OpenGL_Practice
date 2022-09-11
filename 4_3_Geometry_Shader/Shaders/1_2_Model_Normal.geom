#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

const float scale = 0.2;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 normal_matrix = transpose(inverse(view*model));
	for(int i=0; i<1; ++i)
	{
		vec4 normal_view = normalize(normal_matrix* vec4(gs_in[i].normal, 0.0));
		vec4 view_model_space = view*model*gl_in[i].gl_Position;
		vec4 normal_dir = normal_view * scale;

		gl_Position = projection * view_model_space;
		EmitVertex();

		gl_Position = projection * (view_model_space + normal_dir);
		EmitVertex();

		EndPrimitive(); // Combines all the emitted vertices into one primitve
	}
}