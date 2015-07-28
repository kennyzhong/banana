#version 430

in vec3 position;
in vec3 normal;
in mat4 world_instanced;
in vec3 color_instanced;

out vec3 normal0;
out vec3 normal_original;
out vec3 color_instanced0;

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 world;
uniform int instanced;

void main()
{
	mat4 mvp;
	if(instanced == 1)
		mvp = projection * camera * world * world_instanced;
	else
		mvp = projection * camera * world;

	gl_Position =  mvp * vec4(position, 1.0);
	normal_original = normal;
	normal0 =  (mvp * vec4(normal, 0.0)).xyz;
	color_instanced0 = color_instanced;
}