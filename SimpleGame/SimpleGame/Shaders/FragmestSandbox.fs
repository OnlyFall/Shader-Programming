#version 330

layout(location=0) out vec4 FragColor;

uniform vec2 u_Point;
uniform vec2 u_Points[3];
uniform float u_Time;

in vec2 v_UV;

const float c_PI = 3.141592;

void Circle()
{
	vec2 newValue = v_UV - u_Points[2];
	float distance = length(newValue);
	if(distance < 0.1f)
	{
		FragColor = vec4(v_UV.xy, 0.0f, 1.0f);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void Circles()
{
	vec2 newValue = v_UV - u_Point;
	float distance = length(newValue);
	FragColor = vec4(sin(30*distance));
}

void rader()
{
	
	vec2 newValue = v_UV - u_Point;
	float d = length(newValue);
	float value = sin(4*c_PI*d - 13 * u_Time) - 0.9;
	float ring_mask = ceil(value);


	float obj_mask = 0.0f;
	for(int i = 0; i < 3; i++)
	{
		vec2 temp = v_UV - u_Points[i];
		float d = length(temp);

		if(d < 0.1f)
		{
			obj_mask += 1.0;
		}
	}
	FragColor = vec4(ring_mask * obj_mask + 10 * value);
}

void main()
{
	rader();
	//Circle();
}
