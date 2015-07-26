#version 430

in vec3 normal0;

out vec4 gl_FragColor;

uniform vec4 color;

void main()
{
//* clamp(dot(-vec3(0.0, 0.0, 1.0), normal0), 0.0, 1.0)
//vec4(1.0 * normal0.r, 1.0 * normal0.g, 1.0 * normal0.b, 1.0)
	gl_FragColor = color ;
}