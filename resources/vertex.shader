#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

uniform mat4 modelViewProjection;

out vec3 outColor;

void main()
{
  gl_Position = modelViewProjection * vec4(aPosition, 1.0f);
  outColor = aColor;
} 