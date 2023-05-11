#version 330

layout(location=0) out vec4 FragColor;

uniform sampler2D u_TexSampler;
uniform vec2 u_XYRepeat;

in vec2 v_TexPos;

void def()
{
	FragColor = vec4(v_TexPos, 0, 1);
}

void example1()
{
	float x = v_TexPos.x;
	float y = 1.0 - abs(v_TexPos.y * 2.0 - 1.0);
	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);

}

void example2()
{
	float x = v_TexPos.x * 3;
	float y;
	y = (v_TexPos.y / 3) + 1.f/3.f;
	vec2 newTexPos = vec2(x,y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void example3()
{
	float x = v_TexPos.x * 3;
	float y;
	if (x <= 1)
		y = v_TexPos.y / 3;
	else if (x <= 2)
	{
		y = (v_TexPos.y / 3) + (1.f/3.f) + 0.01;
		if(y >= (2.f/3.f))
			y = (v_TexPos.y / 3) + 0.02;
	}
	else if(x <= 3)
	{
		y = (v_TexPos.y / 3) + (2.f/3.f) + 0.01;
		if(y >= 1)
			y = 0.99;
	}

	vec2 newTexPos = vec2(x,y);
	FragColor = texture(u_TexSampler, newTexPos);

}

void P5()
{
	float x_repeat = u_XYRepeat.x;
	float y_repeat = u_XYRepeat.y;

	float dx = v_TexPos.x * x_repeat;
	float x = fract(dx + floor((1.0 - v_TexPos.y) * y_repeat) * 0.5);
	float y = fract(v_TexPos.y * y_repeat);
	
	vec2 newTexPos = vec2(x, y);
	
	FragColor = texture(u_TexSampler, newTexPos);
}

void P6()
{
	float x_repeat = u_XYRepeat.x;
	float y_repeat = u_XYRepeat.y;

	float dy = v_TexPos.y * y_repeat;
	float x = fract(v_TexPos.x * x_repeat);
	float y = fract(dy + floor(v_TexPos.x * x_repeat) * 0.5);
	
	vec2 newTexPos = vec2(x, y);
	
	FragColor = texture(u_TexSampler, newTexPos);
}

void P7()
{
	//float x_repeat = u_XYRepeat.x;
	//float y_repeat = u_XYRepeat.y;

	float x = fract(v_TexPos.x + v_TexPos.y);
	float y = fract(v_TexPos.x + (1.0 - v_TexPos.y));
	
	vec2 newTexPos = vec2(x, y);
	
	FragColor = texture(u_TexSampler, newTexPos);
}

void main()
{
	P7();
}
