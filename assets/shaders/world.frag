#version 430

in vec2 tex_coord0;
in mat4 tex_offset0;
out vec4 gl_FragColor;
uniform sampler2D tex;

void main()
{
	vec2 tex_coord_real;
 	tex_coord_real = (tex_offset0 * vec4(tex_coord0, 0, 1.0)).xy;
	gl_FragColor = texture(tex, tex_coord_real).rgba;
	//gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}