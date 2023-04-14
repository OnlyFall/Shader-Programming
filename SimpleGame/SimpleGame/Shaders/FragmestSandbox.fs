#version 330

layout(location=0) out vec4 FragColor;

uniform vec4 u_Color;
uniform float u_Time;
uniform vec2 u_Pos;

varying vec2 v_Texcoord;

void Circles()
{
	vec2 newValue = v_Texcoord - u_Pos;
	float distance = length(newValue);
	FragColor = vec4(sin(30*distance));
}

void main()
{
	Circles();
}
