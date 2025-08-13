#version 460 core

layout (location = 0) in vec2 aPos;

uniform mat4 transform;
uniform lowp float pointSize;

void main()
{
    gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
    gl_PointSize = max(1.0, pointSize);
};
