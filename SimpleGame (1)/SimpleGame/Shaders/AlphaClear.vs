#version 330

in vec3 a_Position;


void main()
{
	vec4 newPosition = vec4(a_Position, 1.0);
	gl_Position = newPosition;
}
