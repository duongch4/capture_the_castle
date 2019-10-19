#version 330 

layout(location = 0) in vec4 in_position;

out vec2 texture_coordinate;

void main()
{
    // Convert the vertex coordinate [-1, 1] to texture coordinate [0, 1]
    texture_coordinate = (in_position.xy + vec2(1.0, 1.0)) / 2.0;

	// No need for any fancy matrix transformation since we provided normalized coordinates
	gl_Position = in_position;
}
