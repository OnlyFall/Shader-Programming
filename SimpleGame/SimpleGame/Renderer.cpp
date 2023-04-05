#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
	Class0310();
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_ParticleShader = CompileShaders("./Shaders/Particle.vs", "./Shaders/Particle.fs");

	//Create VBOs
	CreateVertexBufferObjects();
	CreateParticleVBO(1000);
	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string* target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::class0310_Rendering()
{
	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), 0, 0, 0, 1);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), 1, 1, 1, 1);

	int attribLocation_Position = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribLocation_Position); // 이거 나중에 설명해 주신다 함! 
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glVertexAttribPointer(attribLocation_Position, 3, GL_FLOAT, GL_FALSE, 0, 0);


	int attribLocation_Position1 = glGetAttribLocation(m_SolidRectShader, "a_Position1");
	glEnableVertexAttribArray(attribLocation_Position1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glVertexAttribPointer(attribLocation_Position1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	int uniformScale = glGetUniformLocation(m_SolidRectShader, "u_Scale");
	if (up_side_down) {
		scale += 0.0001f;
		if (scale >= 1.f)
			up_side_down = false;
	}
	else {
		scale -= 0.0001f;
		if (scale <= 0)
			up_side_down = true;
	}
	glUniform1f(uniformScale, scale);
	glDrawArrays(GL_TRIANGLES, 0, 3); // 첫번째 인자는 primitive가 됨
}

void Renderer::DrawParticle()
{
	GLuint program = m_ParticleShader;
	glUseProgram(program);

	int posLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int velLoc = glGetAttribLocation(program, "a_Vel");
	glEnableVertexAttribArray(velLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBOvel);
	glVertexAttribPointer(velLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int emitTimeLoc = glGetAttribLocation(program, "a_EmitTime");
	glEnableVertexAttribArray(emitTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glVertexAttribPointer(emitTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int lifeTimeLoc = glGetAttribLocation(program, "a_LifeTime");
	glEnableVertexAttribArray(lifeTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glVertexAttribPointer(lifeTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);


	int periodLoc = glGetAttribLocation(program, "a_Period");
	glEnableVertexAttribArray(periodLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_PatriclePeriodVBO);
	glVertexAttribPointer(periodLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int ampLoc = glGetAttribLocation(program, "a_Amp");
	glEnableVertexAttribArray(ampLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glVertexAttribPointer(ampLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
	g_time += 0.0004; // 해당수치를 조절하시오

	int time_loc = glGetUniformLocation(program, "u_Time");
	glUniform1f(time_loc, g_time);

	int accel_loc = glGetUniformLocation(program, "u_Accel");
	glUniform3f(accel_loc, 0.f, -2.8f, 0.f);  // 3번째 인자를 조절하시오

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVertexCount);

}


void Renderer::GetGLPosition(float x, float y, float* newX, float* newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::Class0310()
{

	float vertices[] = { 0,0,0,1,0,0,1,1,0 };
	float vertices1[] = { -1,-1,0, 0,-1,0, 0,0,0 };

	glGenBuffers(1, &m_testVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // GL_STATIC_DRAW -> 이 옵션이 붙으면 gpu memory로 올리고 이 정보는 절대로 변하지 않는다를 의미함!
	// GL_DYNAMIC_DRAW -> 데이터를 올리고 나서 값이 바뀔 수 있을 때 사용 (성능적으로 볼때는 위아래 큰 차이는 없지만 추가적으로 메모리가 잡힘)

	glGenBuffers(1, &m_testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	float color[] = { 1,0,0,1 ,0,1,0,1 ,0,0,1,1 };

	glGenBuffers(1, &m_colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
}

void Renderer::CreateParticleVBO(int numParticleCount)
{
	int vertexCount = 6; // 파티클당 필요한 버텍스의 갯수!
	int particleCount = numParticleCount;
	int floatCount = 3; // 버텍스당 몆개의 float 포인트의 갯수가 필요한가?
	int totalFloatCount = particleCount * vertexCount * floatCount;
	int totalfloatCountSingle = particleCount * vertexCount * 1;

	m_ParticleVertexCount = particleCount * vertexCount;
	float* vertices = NULL;
	vertices = new float[totalFloatCount];

	float* verticesvef = NULL;
	verticesvef = new float[totalFloatCount];
	float particleSize = 0.01f;


	int index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float particleCenterX = 0.f * (float)((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f);
		float particleCenterY = 0.f * (float)((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f);
		vertices[index] = particleCenterX - particleSize; index++;
		vertices[index] = particleCenterY + particleSize; index++;
		vertices[index] = 0; index++;
		vertices[index] = particleCenterX - particleSize; index++;
		vertices[index] = particleCenterY - particleSize; index++;
		vertices[index] = 0; index++;
		vertices[index] = particleCenterX + particleSize; index++;
		vertices[index] = particleCenterY + particleSize; index++;
		vertices[index] = 0; index++;

		vertices[index] = particleCenterX + particleSize; index++;
		vertices[index] = particleCenterY + particleSize; index++;
		vertices[index] = 0; index++;
		vertices[index] = particleCenterX - particleSize; index++;
		vertices[index] = particleCenterY - particleSize; index++;
		vertices[index] = 0; index++;
		vertices[index] = particleCenterX + particleSize; index++;
		vertices[index] = particleCenterY - particleSize; index++;
		vertices[index] = 0; index++;
	}
	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalFloatCount, vertices, GL_STATIC_DRAW);
	delete[] vertices;
	

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float velX = 2.0f * (float)((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f);
		float velY = 5.0f * (float)((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
		verticesvef[index] = velX; index++;
		verticesvef[index] = velY; index++;
		verticesvef[index] = 0; index++;
		verticesvef[index] = velX; index++;
		verticesvef[index] = velY; index++;
		verticesvef[index] = 0; index++;
		verticesvef[index] = velX; index++;
		verticesvef[index] = velY; index++;
		verticesvef[index] = 0; index++;

		verticesvef[index] = velX; index++;
		verticesvef[index] = velY; index++;
		verticesvef[index] = 0; index++;
		verticesvef[index] = velX; index++;
		verticesvef[index] = velY; index++;
		verticesvef[index] = 0; index++;
		verticesvef[index] = velX; index++;
		verticesvef[index] = velY; index++;
		verticesvef[index] = 0; index++;
	}
	glGenBuffers(1, &m_ParticleVBOvel);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBOvel);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalFloatCount, verticesvef, GL_STATIC_DRAW);
	delete[] verticesvef;


	// EmitTime
	float* verticesEmitTime = NULL;
	verticesEmitTime = new float[totalfloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float emitTime = ((float)rand()) / ((float)RAND_MAX) * 10.f;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
	}
	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalfloatCountSingle, verticesEmitTime, GL_STATIC_DRAW);
	delete[] verticesEmitTime;


	// LifeTime
	float* verticesLifeTime = NULL;
	verticesLifeTime = new float[totalfloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float LifeTime = ((float)rand()) / ((float)RAND_MAX) * 1.f; // 라이프타임 수정은 여기서!
		verticesLifeTime[index] = LifeTime; index++;
		verticesLifeTime[index] = LifeTime; index++;
		verticesLifeTime[index] = LifeTime; index++;
		verticesLifeTime[index] = LifeTime; index++;
		verticesLifeTime[index] = LifeTime; index++;
		verticesLifeTime[index] = LifeTime; index++;
	}
	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalfloatCountSingle, verticesLifeTime, GL_STATIC_DRAW);
	delete[] verticesLifeTime;
	

	m_PatriclePeriodVBO;
	// LifeTime
	float* verticesPeriodTime = NULL;
	verticesPeriodTime = new float[totalfloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float peropd = 1.f * ((float)rand()) / ((float)RAND_MAX) * 1.f; // 라이프타임 수정은 여기서!
		verticesPeriodTime[index] = peropd; index++;
		verticesPeriodTime[index] = peropd; index++;
		verticesPeriodTime[index] = peropd; index++;
		verticesPeriodTime[index] = peropd; index++;
		verticesPeriodTime[index] = peropd; index++;
		verticesPeriodTime[index] = peropd; index++;
	}
	glGenBuffers(1, &m_PatriclePeriodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_PatriclePeriodVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalfloatCountSingle, verticesPeriodTime, GL_STATIC_DRAW);
	delete[] verticesPeriodTime;

	float* verticesAmpTime = NULL;
	verticesAmpTime = new float[totalfloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float Amp = 1.f * (2.f * ((float)rand()) / ((float)RAND_MAX) - 1.f); // 라이프타임 수정은 여기서!
		verticesAmpTime[index] = Amp; index++;
		verticesAmpTime[index] = Amp; index++;
		verticesAmpTime[index] = Amp; index++;
		verticesAmpTime[index] = Amp; index++;
		verticesAmpTime[index] = Amp; index++;
		verticesAmpTime[index] = Amp; index++;
	}
	glGenBuffers(1, &m_ParticleAmpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalfloatCountSingle, verticesAmpTime, GL_STATIC_DRAW);
	delete[] verticesAmpTime;



}