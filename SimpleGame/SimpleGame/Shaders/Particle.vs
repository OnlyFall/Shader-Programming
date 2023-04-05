#version 330

in vec3 a_Position;
in vec3 a_Vel;
in float a_EmitTime;
in float a_LifeTime;
in float a_Period;
in float a_Amp;

uniform float u_Time;
uniform vec3 u_Accel;


const vec3 c_Vel = vec3(1.0, 0.0, 0.0);
const float c_resetTime = 2.0;
const float c_PI = 3.141592;
const float c_Amp = 0.5;
const float c_Period = 2;


vec4 P1()
{
	vec4 newPosition = vec4(0, 0, 0, 1);

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

	return newPosition;
}

vec4 GraphSin()
{
	
	vec4 newPos = vec4(0,0,0,1);
	float t = u_Time - a_EmitTime;
	if(t < 0.0)
	{
		
	}
	else
	{
		float newT = a_LifeTime * fract(t/a_LifeTime);
		newPos.x = a_Position.x + c_Vel.x * newT;
		newPos.y = a_Position.y + a_Amp * sin(a_Period * newT * 2.0 * c_PI); // SIN 2주기를 하고싶다! 자주내는 부분임!
	}

	return newPos;
}


void main()
{
	gl_Position = GraphSin();
}
