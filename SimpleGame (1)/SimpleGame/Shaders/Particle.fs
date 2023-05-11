#version 330

layout(location=0) out vec4 FragColor;

in vec4 v_Color;
in vec2 v_UV;

void Circles()
{
	vec2 newValue = v_UV - vec2(0.5f, 0.5f);
	float distance = length(newValue);
	if(distance < 0.5f){
		FragColor = vec4(v_Color);
	}
	else{
		FragColor = vec4(0);
	}
}

void main()
{
	FragColor = vec4(1);
	//FragColor = v_Color;
	//FragColor = vec4(v_UV, 0, 1);
	//Circles();
}
