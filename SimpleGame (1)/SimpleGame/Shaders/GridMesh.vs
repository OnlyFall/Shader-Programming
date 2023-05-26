#version 330

const float PI = 3.141592;

in vec3 a_Position;

uniform float u_Time;

out float v_greyScale;
out vec2 v_TexPos;

void main()
{
	float x = a_Position.x;
	float value =  (a_Position.x + 0.5) * 2.0 * PI - u_Time;
	float temp = a_Position.x + 0.5;

	float y = a_Position.y + 0.25 * temp *sin(value);
	vec4 newPosition = vec4(x, y, 0, 1.0);
	vec3 targetPos = vec3(1.0, -1.0, 0.0);

	float timeY = length(a_Position.xy - targetPos.xy);
	// (a_Position.x + 0.5)
	// (a_Position.y + 0.5)
	// 1 - (a_Position.y + 0.5)
	float newTime = u_Time - timeY;
	newTime = max(0, newTime);

	vec3 morphPos = mix(newPosition.xyz, targetPos, newTime);
	gl_Position = vec4(morphPos, 1);

	v_greyScale = 0.3 + (sin(value) + 1.0) / 2.0;
	
	float tx = a_Position.x + 0.5; // 0 ~ 1°ªÀ¸·Î!
	float ty = 1.0 - (a_Position.y + 0.5);
	v_TexPos = vec2(tx, ty);
}
