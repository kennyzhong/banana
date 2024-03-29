#version 430

in vec3 normal0;
in vec3 color_instanced0;

out vec4 gl_FragColor;

uniform vec4 color;
uniform int instanced;
uniform vec3 light_direction;
uniform mat4 camera;
uniform mat4 projection;
uniform mat4 world;

void main()
{

//vec4(1.0 * normal0.r, 1.0 * normal0.g, 1.0 * normal0.b, 1.0)
 //* clamp(dot(-vec3(0.0, 0.0, 1.0), normal0), 0.0, 1.0)
 	vec4 final_color;
 	if(instanced == 1)
 		final_color = vec4(color_instanced0, 1.0);
 	else
 		final_color = color;

	//normal_original.x = clamp(normal_original.x, 0.0, 1.0);
	//normal_original.y = clamp(normal_original.y, 0.0, 1.0);
	//normal_original.z = clamp(normal_original.z, 0.0, 1.0);
	
	//vec3 ld = (projection * camera * vec4(light_direction, 0.0)).xyz;

	gl_FragColor = final_color * clamp(dot(-light_direction, normal0), 0.1, 1.0);
}