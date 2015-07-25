#version 430

in vec2 tex_coord0;
in int id;
out vec4 gl_FragColor;
uniform vec4 color;
uniform sampler2D tex;
uniform int using_tex;
uniform mat4 tex_offset;

void main()
{
	vec2 tex_coord_real;
 	tex_coord_real = (tex_offset * vec4(tex_coord0, 0, 1.0)).xy;
 	vec4 final_color;
	if(using_tex == 1)
		final_color = color * texture(tex, vec2(tex_coord_real)).rgba;
	else
		final_color = color;
	gl_FragColor = final_color;
}