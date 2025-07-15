#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

uniform mat3 projection;

out vec2 TexCoords;

void main()
{
vec3 transformed = projection * vec3(vertex.xy, 1.0);

gl_Position = vec4(vertex.xy, 0.0, 1.0);

TexCoords = vertex.zw;
}
