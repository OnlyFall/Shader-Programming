#version 330

const float PI = 3.141592;

in vec3 a_Position;

uniform float u_Time;

out float v_greyScale;

void main()
{
	float x = a_Position.x;
	float value =  (a_Position.x + 0.5) * 2.0 * PI - u_Time;
	float temp = a_Position.x + 0.5;

	float y = a_Position.y + 0.25 * temp *sin(value);
	vec4 newPosition = vec4(x, y, 0, 1.0);
	gl_Position = newPosition;
	v_greyScale = (sin(value) + 1.0) / 2.0;
}
