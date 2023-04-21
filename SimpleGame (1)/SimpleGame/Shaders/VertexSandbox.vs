#version 330

in vec3 a_Position;

const float PI = 3.141592;

uniform float u_Time;

out float v_Alpha;

void sinLine()
{
	float newAlpha = 1 - (a_Position.x + 1.0) / 2.0;
	float value = (a_Position.x + 1.0f) * PI - u_Time;
	float newY = sin(value);
	float d = a_Position.x + 1.0f;

	vec4 newPos = vec4(a_Position, 1.0);
	newPos.y = newY * (d * 0.5);
	
	gl_Position = newPos;
	v_Alpha = newAlpha;
}

void main()
{
	sinLine();
}
