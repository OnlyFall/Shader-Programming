#version 330

in vec3 a_Position;
in vec3 a_Vel;
in float a_EmitTime;

uniform float u_Time;
uniform vec3 u_Accel;

const vec3 c_Vel = vec3(0.001, 0.0, 0.0);
const float c_resetTime = 2.0;

void main()
{
	vec4 newPosition = vec4(0, 0, 0, 1);

	float t = u_Time - a_EmitTime;
	if(t < 0.0)
	{
		
	}
	else
	{
		float newT = c_resetTime * fract(t/c_resetTime);
		newPosition.xyz = a_Position 
							+ a_Vel * newT 
							+ 0.5 * u_Accel * newT * newT;
		newPosition.w= 1;
	}

	gl_Position = newPosition;
}
