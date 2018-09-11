#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

varying vec2 textureOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	textureOut = vec2(aTexCoord.x, aTexCoord.y);
}
