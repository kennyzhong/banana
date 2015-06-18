#version 430

in vec2 position;
in vec2 tex_coord;
out vec2 tex_coord0;
out int id;
uniform mat4 projection;
uniform mat4 camera;
uniform mat4 world;

void main()
{
	gl_Position = projection * camera * world * vec4(position, 0.0f, 1.0);
	tex_coord0 = tex_coord;
	id = gl_VertexID;
}