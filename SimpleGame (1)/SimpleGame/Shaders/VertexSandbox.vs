#version 330

in vec3 a_Position;

const float PI = 3.141592;


void main()
{
	float value = (a_Position.x + 1.0f) * PI;
	float newY = sin(value);
	vec4 newPos = vec4(a_Position, 1.0);
	newPos.y = newY;
	gl_Position = newPos;
}
