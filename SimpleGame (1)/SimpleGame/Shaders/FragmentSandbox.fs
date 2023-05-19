#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_UV;

uniform vec2 u_Point;
uniform vec2 u_Points[3];
uniform float u_Time;

uniform sampler2D u_Texture;

const float c_PI = 3.141592;

void circle()
{
	vec2 newValue = v_UV - u_Points[2];
	float d = length(newValue);
	if(d < 0.1f)
	{
		FragColor = vec4(v_UV.xy, 0.0, 1.0);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void circles()
{
	vec2 newValue = v_UV - u_Point;
	float d = length(newValue);
	FragColor = vec4(sin(30*d));
}

void radar()
{
	vec2 newValue = v_UV - vec2(0.5, 1);
	float d = length(newValue);
	float value = sin(2*c_PI*d - 13*u_Time) - 0.93;
	float ring_mask = ceil(value);

	float obj_mask = 0.0;

	for(int i=0; i<3; i++)
	{
		vec2 temp = v_UV - u_Points[i];
		float d = length(temp);
		if(d<0.05)
		{
			obj_mask += 1.0;
		}
	}
	FragColor = vec4(ring_mask*obj_mask + 10*value);
	
}

void UVTest()
{
	FragColor = vec4(0);
	float powValue = 100.0;
	float sinResultX = pow(sin(v_UV.x * 10 * c_PI), powValue);
	float sinResultY = pow(sin(v_UV.y * 10 * c_PI), powValue);
	float finalResult = max(sinResultX, sinResultY);
	FragColor = vec4(finalResult);
}

void sinGraph()
{
	FragColor = vec4(0);
	for(int i=0; i<5; i++)
	{
		float newTime = u_Time + i*0.2;
		vec2 newUV = vec2(v_UV.x, 2.0*(v_UV.y-0.5));
		float newInput = v_UV.x*2*c_PI + c_PI; //0~2PI
		float sinValue = 0.5*v_UV.x*sin(newInput - newTime*5);
		float width = 0.005;
		float newAlpha = 1.0 - v_UV.x;
		float newLines = sin(v_UV.x*200.0 - newTime*50);
		if(newUV.y > sinValue && newUV.y < sinValue+width)
		{
			FragColor += vec4(1.0*newAlpha*newLines);
		}
	}
}

void flag()
{
	float finalColor = 0;
	for(int i = 0; i < 10; i++)
	{
		float newTime = u_Time + i * 0.2;
		float newColor = v_UV.x * 0.5 *
							sin(v_UV.x * c_PI * 2 - 10 * newTime);
		float sinValue = sin(v_UV.x * c_PI * 2 * 10 - 500 * newTime);
		float width = 0.01 * v_UV.x * 5 + 0.001;

		if(2.0 * (v_UV.y - 0.5) > newColor && 2.0 * (v_UV.y - 0.5) < newColor + width)
		{
			finalColor += 1 * sinValue * (1.0 - v_UV.x);
		}
		else
		{
		}
	}

	FragColor = vec4(finalColor);
}

void realFlag()
{
	float period = (v_UV.x + 1.0) * 1.0;
	float xValue = v_UV.x * 2.0 * c_PI * period;
	float yValue = ((1.0 - v_UV.y) - 0.5) * 2.0;
	float sinValue = 0.25 * sin(xValue - 2 * u_Time);
	
	if(sinValue * v_UV.x + 0.75 > yValue && sinValue * v_UV.x - 0.75 < yValue)
	{
		float vX = v_UV.x;
		float yWidth = 1.5;
		float yDistance = yValue - (sinValue * v_UV.x - 0.75);
		float vY = 1.0 - yDistance / yWidth;
		FragColor = texture(u_Texture, vec2(vX, vY));
		//FragColor = vec4(vX, vY, 0, 1);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void main()
{
	//radar();
	//UVTest();
	//sinGraph();
	realFlag();
}
