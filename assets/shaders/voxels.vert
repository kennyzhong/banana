#version 430

in vec3 position;
in vec3 normal;

out vec3 normal0;

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 world;

void main()
{
	gl_Position = projection * camera * world * vec4(position, 1.0);
	normal0 =  (projection * camera * world * vec4(normal, 0.0)).xyz;
}