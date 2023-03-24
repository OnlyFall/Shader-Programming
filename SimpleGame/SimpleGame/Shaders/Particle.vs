#version 330

in vec3 a_Position;
in vec3 a_Vel;
in float a_EmitTime;
in float a_LifeTime;

uniform float u_Time;
uniform vec3 u_Accel;

const vec3 c_Vel = vec3(1.0, 0.0, 0.0);
const float c_resetTime = 2.0;

void main()
{
	vec4 newPos = vec4(0,0,0,1);
	float t = fract(u_Time);
	newPos.xyz  = a_Position + c_Vel * t; 

	gl_Position = newPos;

	/*vec4 newPosition = vec4(0, 0, 0, 1);

	float t = u_Time - a_EmitTime;
	if(t < 0.0)
	{
		
	}
	else
	{
		float newT = a_LifeTime * fract(t/a_LifeTime);
		newPosition.xyz = a_Position 
							+ a_Vel * newT 
							+ 0.5 * u_Accel * newT * newT;
		newPosition.w= 1;
	}

	gl_Position = newPosition;*/
}
