#version 330

const float PI = 3.141592;

in vec3 a_Position;

void main()
{
	float x = a_Position.x;
	float value =  (a_Position.x + 0.5) * 2.0 * PI;
	float y = a_Position.y + 0.5 * sin(value);
	vec4 newPosition = vec4(x, y, 0, 1.0);
	gl_Position = newPosition;
}
