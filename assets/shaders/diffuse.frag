#version 430

in vec2 tex_coord0;
in int id;
out vec4 gl_FragColor;
uniform vec4 color;
uniform sampler2D tex;
uniform int using_tex;
uniform mat4 tex_offset;
uniform vec3 color_change;

void main()
{
	vec2 tex_coord_real;
 	tex_coord_real = (tex_offset * vec4(tex_coord0, 0, 1.0)).xy;
 	vec4 final_color;
	if(using_tex == 1)
		final_color = texture(tex, vec2(tex_coord_real)).rgba;
	else
		final_color = color;
	final_color.r *= color_change.r;
	final_color.g *= color_change.g;
	final_color.b *= color_change.b;
	gl_FragColor = final_color;
}