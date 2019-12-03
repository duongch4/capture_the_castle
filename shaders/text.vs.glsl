#version 330

in vec4 vertex; // <vec2 position, vec2 texturecoord>
out vec2 textcoord;

uniform mat3 projection;

void main() {
    textcoord = vertex.zw;
    vec3 pos = projection * vec3(vertex.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
