#version 460 core

out vec4 FragColor;

uniform sampler2D image;
uniform vec3 colour;
uniform float pointSize;

void main()
{
   if (pointSize > 5.0)
      FragColor = texture(image, gl_PointCoord) * vec4(colour, 1.0f);
   else
      FragColor = vec4(colour, 1.0f);
};