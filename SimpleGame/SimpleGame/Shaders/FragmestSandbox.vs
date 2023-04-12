#version 330

in vec3 a_Position;
in vec2 a_Texcoord;

varying vec2 v_Texcoord;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
	float tx = (a_Position.x + 1.0) / 2.0;
	float ty = (-a_Position.y + 1.0) / 2.0;

	v_Texcoord = vec2(tx, ty);
	//v_Texcoord = a_Texcoord;
}
