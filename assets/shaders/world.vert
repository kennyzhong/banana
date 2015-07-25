#version 430

in vec2 position;
in mat4 world;
in vec2 tex_coord;
in mat4 tex_offset;
out vec2 tex_coord0;
out mat4 tex_offset0;
uniform mat4 projection;
uniform mat4 camera;

void main()
{
	gl_Position = projection * camera * world * vec4(position, 0.0, 1.0);
	tex_coord0 = tex_coord;
	tex_offset0 = tex_offset;
}
