#version 330

uniform sampler2D screen_texture;

in vec2 texture_coordinate;

layout(location = 0) out vec4 color;

void main()
{
	// Sample from the entire screen texture
    color = texture(screen_texture, texture_coordinate);
}