#version 330

// Input color
in vec3 v_color;

// Output color
layout(location = 0) out  vec4 out_color;

void main()
{
	out_color = vec4(v_color, 1.0);
}
