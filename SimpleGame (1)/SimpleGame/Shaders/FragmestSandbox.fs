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
	
	vec2 newValue = v_UV - vec2(0.5, 0.5);
	float d = length(newValue);
	float value = sin(2*c_PI*d - 13 * u_Time) - 0.9;
	float ring_mask = ceil(value);


	float obj_mask = 0.0f;
	for(int i = 0; i < 3; i++)
	{
		vec2 temp = v_UV - u_Points[i];
		float d = length(temp);

		if(d < 0.1)
		{
			obj_mask += 1.0;
		}
	}
	FragColor = vec4(ring_mask * obj_mask + 10 * value);
}

void uvTest()
{
	FragColor = vec4(0);
	float powValue = 10;
	float sinResultX = pow(sin(v_UV.x * 10 * c_PI), powValue);
	float sinResultY = pow(sin(v_UV.y * 10 * c_PI), powValue);
	float finalResult = max(sinResultX, sinResultY);

	FragColor = vec4(finalResult);
}

void sinGraph()
{
	FragColor = vec4(0);
	for(int i = 0; i < 5; i++)
	{
		float newTime = u_Time + i * 0.2;
		vec2 newUV = vec2(v_UV.x, 2.0 * (v_UV.y - 0.5));

	float newInput = v_UV.x * 2 * c_PI + c_PI;
	float sinValue = v_UV.x * sin(newInput - newTime * 20);

	float width = 0.005;
	float newAlpha = 1.0 - v_UV.x;
	float newLines = sin(v_UV.x * 200.0 - newTime * 50);

	if(newUV.y > sinValue && newUV.y < sinValue + width)
		FragColor += vec4(1.0 * newAlpha * newLines);
	}
}

void main()
{
	sinGraph();
	//uvTest();
	//rader();
	//Circle();
}
