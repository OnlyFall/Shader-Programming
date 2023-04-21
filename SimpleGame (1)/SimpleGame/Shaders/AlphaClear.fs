#version 330

layout(location=0) out vec4 FragColor;

varying vec4 v_Color;

void main()
{
	FragColor = vec4(0, 0, 0, 0.1);
}
