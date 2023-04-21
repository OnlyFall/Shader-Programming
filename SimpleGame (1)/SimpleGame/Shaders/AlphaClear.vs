#version 330

in vec3 a_Position; //ATTRIBUTE (VS INPUT)

void main()
{
	gl_Position = vec4(a_Position, 1);
}