#version 330

layout(location=0) out vec4 FragColor;

uniform vec4 u_Color;
uniform float u_Time;

varying vec2 v_Texcoord;

void Circles()
{
	vec2 newValue = v_Texcoord - vec2(0.5f, 0.5f);
	float distance = length(newValue);
	FragColor = vec4(sin(30*distance));
}

void main()
{
	Circles();
	//vec2 newValue = v_Texcoord - vec2(0.5f, 0.5f);
	//float distance = length(newValue);

	//if(distance < 0.5f)
	//	FragColor = vec4(v_Texcoord.xy, 0.0, 1.0);
	//else
	//	FragColor = vec4(0);
}
