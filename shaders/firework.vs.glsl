#version 330 

// Input attributes
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_translate;
layout (location = 2) in float in_scale;
layout (location = 3) in vec3 in_color;

// Application data
uniform mat3 projection;

// Output
out vec3 v_color;

void main()
{
    v_color = in_color;

  	mat3 transform = mat3(
    	in_scale, 0.0, 0.0,
    	0.0, in_scale, 0.0,
    	in_translate.x, in_translate.y, 1.0);

	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
