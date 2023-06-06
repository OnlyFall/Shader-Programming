#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"
#include <Windows.h>
#include <assert.h>

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
	m_FragmentSandboxShader = CompileShaders("./Shaders/FragmentSandbox.vs", "./Shaders/FragmentSandbox.fs");
	m_AlphaClearShader = CompileShaders("./Shaders/AlphaClear.vs", "./Shaders/AlphaClear.fs");
	m_VertexSandboxShader = CompileShaders("./Shaders/VertexSandbox.vs", "./Shaders/VertexSandbox.fs");
	m_testShader = CompileShaders("./Shaders/VertexSandbox.vs", "./Shaders/VertexSandbox.fs");
	m_TextureSandboxShader = CompileShaders("./Shaders/TextureSandbox.vs", "./Shaders/TextureSandbox.fs");
	m_GridMeshShader = CompileShaders("./Shaders/GridMesh.vs", "./Shaders/GridMesh.fs");
	m_DrawTextureShader = CompileShaders("./Shaders/DrawTexture.vs", "./Shaders/DrawTexture.fs");
	//Create VBOs
	CreateVertexBufferObjects();
	CreateTexures();
	CreateGridMesh();

	// Create FBOs
	CreateFBOs();
	// Prepare bloom
	PrepareBloom();
	//Load Textures
	m_RGBTexture = CreatePngTexture("./rgb.png", GL_NEAREST);



	m_ParticleTexture = CreatePngTexture("./particle.png", GL_NEAREST);
	m_ExplosionTexture = CreatePngTexture("./explosion.png", GL_NEAREST);
	m_rabbit_texture = CreatePngTexture("./keyescape.png", GL_NEAREST);

	m_0Texture = CreatePngTexture("./Texture0.png", GL_NEAREST);
	m_1Texture = CreatePngTexture("./Texture1.png", GL_NEAREST);
	m_2Texture = CreatePngTexture("./Texture2.png", GL_NEAREST);
	m_3Texture = CreatePngTexture("./Texture3.png", GL_NEAREST);
	m_4Texture = CreatePngTexture("./Texture4.png", GL_NEAREST);
	m_5Texture = CreatePngTexture("./Texture5.png", GL_NEAREST);

	m_MergeTexture = CreatePngTexture("./AllInOne.png", GL_NEAREST);

	CreateParticleVBO(10000);

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
	float textureRect[]
		=
	{
		-0.5f, 0.5f, 0.f,		0.f, 0.f, //x,y,z,u,v
		-0.5f, -0.5f, 0.f,		0.f, 1.f,
		0.5f, 0.5f, 0.f,		1.f, 0.f, //Triangle1
		0.5f, 0.5f, 0.f,		1.f, 0.f,
		-0.5, -0.5f, 0.f,		0.f, 1.f,
		0.5f, -0.5f, 0.f,		1.f, 1.f //Triangle2
	};
	glGenBuffers(1, &m_TextureSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextureSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureRect), textureRect, GL_STATIC_DRAW);

	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	float rect1[]
		=
	{
		-1.f, -1.f, 0.f, 0.f, 1.f, //x,y,z,u,v
		-1.f, 1.f, 0.f, 0.f, 0.f,
		1.f, 1.f, 0.f, 1.f, 0.f, //Triangle1
		-1.f, -1.f, 0.f,  0.f, 1.f,
		1.f, 1.f, 0.f, 1.f, 0.f,
		1.f, -1.f, 0.f, 1.f, 1.f //Triangle2
	};

	glGenBuffers(1, &m_FragmentSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect1), rect1, GL_STATIC_DRAW);

	float rect2[]
		=
	{
		-1.f, -1.f, 0.f, //x,y,z
		-1.f, 1.f, 0.f,  
		1.f, 1.f, 0.f,  //Triangle1
		-1.f, -1.f, 0.f,  
		1.f, 1.f, 0.f, 
		1.f, -1.f, 0.f //Triangle2
	};

	glGenBuffers(1, &m_AlphaClearVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_AlphaClearVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect2), rect2, GL_STATIC_DRAW);

	m_HorizontalLineVertexCount = 8;
	float* verticesLine = new float[m_HorizontalLineVertexCount * 3];
	float gap = 2.f / ((float)m_HorizontalLineVertexCount - 1.f);
	int index = 0;

	for (int i = 0; i < m_HorizontalLineVertexCount; i++)
	{
		verticesLine[index] = (float)i*gap - 1.f;
		index++;
		verticesLine[index] = 0.f;
		index++;
		verticesLine[index] = 0.f;
		index++;
	}
	glGenBuffers(1, &m_HorizontalLineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_HorizontalLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_HorizontalLineVertexCount*3,
						verticesLine, GL_STATIC_DRAW);
	delete[] verticesLine;

	//// ������=====================================================================
	//index = 0;
	//int circle_line_vertex_count = 2;
	//float* vertices_circle_line = new float[circle_line_vertex_count * 3];

	//for (int i = 0; i < 2; ++i)
	//{
	//	vertices_circle_line[index] = i;
	//	index++;
	//	vertices_circle_line[index] = 0.f;
	//	index++;
	//	vertices_circle_line[index] = 0.f;
	//	index++;
	//}
	//glGenBuffers(1, &m_practiceVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, m_practiceVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_line_vertex_count * 3, vertices_circle_line, GL_STATIC_DRAW);
	//delete[] vertices_circle_line;
	//// ������=====================================================================

	float sizeX = 1.f / m_WindowSizeX;
	float sizeY = 1.f / m_WindowSizeY;

	float drawTextureRect[] =
	{
		-sizeX, sizeY, 0.f,		0.f, 0.f,
		-sizeX, -sizeY, 0.f,	0.f, 1.f,
		sizeX, sizeY, 0.f,		1.f, 0.f,
		sizeX, sizeY, 0.f,		1.f, 0.f,
		-sizeX, -sizeY, 0.f,	0.f, 1.f,
		sizeX, -sizeY, 0.f,		1.f, 1.f,
	};

	glGenBuffers(1, &m_DrawTextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_DrawTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(drawTextureRect), drawTextureRect, GL_STATIC_DRAW);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, p, Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram �� attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
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
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs �� fs ������ �ε���
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(ShaderProgram);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program �α׸� �޾ƿ�
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

float g_time = 0.f;

void Renderer::Class0310_Rendering()
{	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), 0, 0, 0, 1);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), 1, 1, 1, 1);

	int attribLocation_Position = -1;
	attribLocation_Position = glGetAttribLocation(m_SolidRectShader,
		"a_Position");
	glEnableVertexAttribArray(attribLocation_Position);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glVertexAttribPointer(attribLocation_Position, 3, GL_FLOAT, 
		GL_FALSE, 0, 0);

	int attribLocation_Position1 = glGetAttribLocation(m_SolidRectShader, 
		"a_Position1");
	glEnableVertexAttribArray(attribLocation_Position1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glVertexAttribPointer(attribLocation_Position1, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 3, 0);

	int uniformScale = glGetUniformLocation(m_SolidRectShader, "u_Scale");
	glUniform1f(uniformScale, g_time);
	g_time += 0.016;
	if (g_time > 1.f)
		g_time = 0.f;

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::DrawGaussianBlur(GLuint texID, GLuint targetFBOID, GLuint shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, targetFBOID);
	glUseProgram(shader);

	GLuint posLoc = glGetAttribLocation(shader, "a_Position");
	//std::cout << "Gaussian pos loc : " << posLoc << std::endl;
	glEnableVertexAttribArray(posLoc);
	GLuint texLoc = glGetAttribLocation(shader, "a_TexPos");
	glEnableVertexAttribArray(texLoc);
	glUniform1i(glGetUniformLocation(shader, "u_Texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	glBindBuffer(GL_ARRAY_BUFFER, m_FullRectVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawMergeBloomTexture(GLuint sceneTexID, GLuint bloomTexID, float exposure)
{
	GLuint shader = m_DrawMergeTextureShader;
	glUseProgram(shader);

	GLuint posLoc = glGetAttribLocation(shader, "a_Position");
	//std::cout << "merge pos loc : " << posLoc << std::endl;
	glEnableVertexAttribArray(posLoc);
	GLuint texLoc = glGetAttribLocation(shader, "a_TexPos");
	glEnableVertexAttribArray(texLoc);
	glUniform1i(glGetUniformLocation(shader, "u_TextureScene"), 0);
	glUniform1i(glGetUniformLocation(shader, "u_TextureBloom"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomTexID);

	GLuint exposureLoc = glGetUniformLocation(shader, "u_Exposure");
	glUniform1f(exposureLoc, exposure);

	glBindBuffer(GL_ARRAY_BUFFER, m_FullRectVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawParticleWithBloom()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);
	GLenum drawbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawbuffers);
	glClear(GL_COLOR_BUFFER_BIT);
	DrawParticle();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawGaussianBlur(m_HDRLowTexture, m_PingpongFBO[0], m_GaussianBlurHShader);
	for (int i = 0; i < 4; i++)
	{
		DrawGaussianBlur(m_PingpongTexture[0], m_PingpongFBO[1], m_GaussianBlurVShader);
		DrawGaussianBlur(m_PingpongTexture[1], m_PingpongFBO[0], m_GaussianBlurHShader);
	}
	DrawGaussianBlur(m_PingpongTexture[0], m_PingpongFBO[1], m_GaussianBlurVShader);

	DrawTexture(0, 0, 1024, 1024, m_PingpongTexture[1]);
	//DrawTexture(0.5, -0.5, 512, 512, m_HDRLowTexture);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//DrawMergeBloomTexture(m_HDRLowTexture, m_PingpongTexture[1], 1.f);
}

void Renderer::DrawParticle()
{
	//glViewport(0, 0, 1024, 1024);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint program = m_ParticleShader;
	glUseProgram(program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*
	int posLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE,
							0, 0);

	int colorLoc = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(colorLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleColorVBO);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE,
		0, 0);
	*/
	int posLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc);
	int colorLoc = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(colorLoc);
	int uvLoc = glGetAttribLocation(program, "a_UV");
	glEnableVertexAttribArray(uvLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColUVVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float)*9, 0);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT,  GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float)*3));
	glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 7));

	int velLoc = glGetAttribLocation(program, "a_Vel");
	glEnableVertexAttribArray(velLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glVertexAttribPointer(velLoc, 3, GL_FLOAT, GL_FALSE,0, 0);

	int emitTimeLoc = glGetAttribLocation(program, "a_EmitTime");
	glEnableVertexAttribArray(emitTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glVertexAttribPointer(emitTimeLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int lifeTimeLoc = glGetAttribLocation(program, "a_LifeTime");
	glEnableVertexAttribArray(lifeTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glVertexAttribPointer(lifeTimeLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int periodLoc = glGetAttribLocation(program, "a_Period");
	glEnableVertexAttribArray(periodLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glVertexAttribPointer(periodLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int ampLoc = glGetAttribLocation(program, "a_Amp");
	glEnableVertexAttribArray(ampLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glVertexAttribPointer(ampLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);

	int valueLoc = glGetAttribLocation(program, "a_Value");
	glEnableVertexAttribArray(valueLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleValueVBO);
	glVertexAttribPointer(valueLoc, 1, GL_FLOAT, GL_FALSE,
		0, 0);


	int timeLoc = glGetUniformLocation(program, "u_Time");
	glUniform1f(timeLoc, g_time);
	int accelLoc = glGetUniformLocation(program, "u_Accel");
	glUniform3f(accelLoc, 0.f, -2.8f, 0.f);

	g_time += 0.01;

	int texULoc = glGetUniformLocation(program, "u_Texture");
	glUniform1i(texULoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVertexCount);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
}

void Renderer::DrawAlphaClear()
{
	GLuint shader = m_AlphaClearShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_AlphaClearVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawTextureSandbox()
{
#if TEST_MODE
	glBindFramebuffer(GL_FRAMEBUFFER, m_D_FBO);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif

	GLuint shader = m_TextureSandboxShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint posLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posLoc);
	GLuint texLoc = glGetAttribLocation(shader, "a_TexPos");
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_TextureSandboxVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_0Texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_1Texture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_2Texture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_3Texture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_4Texture);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_5Texture);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_MergeTexture);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_ExplosionTexture);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, m_RGBTexture);

	int texID[] = { 0, 1 };
	GLuint samplerULoc = glGetUniformLocation(shader, "u_MultiSampler");
	glUniform1iv(samplerULoc, 2, texID);

	//std::cout << ((int)g_time) % 6 << std::endl;
	samplerULoc = glGetUniformLocation(shader, "u_TexSampler");
	glUniform1i(samplerULoc, 8);
	GLuint stepULoc = glGetUniformLocation(shader, "u_Step");
	glUniform1f(stepULoc, static_cast<float>(static_cast<int>(g_time) % 6));
	//std::cout << static_cast<float>(static_cast<int>(g_time) % 6) << std::endl;
	GLuint seqNumLoc = glGetUniformLocation(shader, "u_SeqNum");
	glUniform1f(seqNumLoc, g_time);
	g_time += 0.04;
	GLuint repeatULoc = glGetUniformLocation(shader, "u_XYRepeat");
	glUniform2f(repeatULoc, 4.f, 4.f);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void Renderer::DrawVSFrag()
{

}

void Renderer::DrawGridMesh()
{
#if TEST_MODE
	glBindFramebuffer(GL_FRAMEBUFFER, m_C_FBO);
	glViewport(0, 0, 1024, 1024);
	GLenum drawBuffers = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, &drawBuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
	GLuint shader = m_GridMeshShader;
	glUseProgram(shader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);

	GLuint posLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint timeLoc = glGetUniformLocation(shader, "u_Time");
	glUniform1f(timeLoc, g_time);
	g_time += 0.0008;

	int uniformLoc_Texture = glGetUniformLocation(shader, "u_Texture");
	glUniform1f(uniformLoc_Texture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_rabbit_texture);

	glDrawArrays(GL_TRIANGLES, 0, m_GridMeshVertexCount);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	DrawTexture(-0.5, -0.5, 512, 512, m_CFBOTexture);
}

void Renderer::DrawVertexSandbox()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_C_FBO);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint shader = m_VertexSandboxShader;
	glUseProgram(shader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_HorizontalLineVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int timeULoc = glGetUniformLocation(shader, "u_Time");
	glUniform1f(timeULoc, g_time);
	g_time += 0.016;

	for (int i = 0; i < 5; i++)
	{
		glUniform1f(timeULoc, g_time+(float)i*0.2);
		glDrawArrays(GL_LINE_STRIP, 0, m_HorizontalLineVertexCount);
	}

	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::DrawTest()
{
	GLuint program = m_testShader;
	glUseProgram(program);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_practiceVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINE_STRIP, 0, 6);
	glDisable(GL_BLEND);
}

void Renderer::DrawResult()
{
	DrawTexture(-0.5, 0.5, 512, 512, m_AFBOTexture);
	DrawTexture(0.5, 0.5, 512, 512, m_BFBOTexture);
	DrawTexture(-0.5, -0.5, 512, 512, m_CFBOTexture);
	DrawTexture(0.5, -0.5, 512, 512, m_DFBOTexture);
}

void Renderer::DrawFragmentSandbox()
{
#if TEST_MODE
	glBindFramebuffer(GL_FRAMEBUFFER, m_A_FBO);
	glViewport(0, 0, 1024, 1024);
	GLenum drawBuffers = { GL_COLOR_ATTACHMENT0 };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif

	GLuint shader = m_FragmentSandboxShader; 
	glUseProgram(shader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLoc = glGetAttribLocation(shader, "a_Position");
	int texLoc = glGetAttribLocation(shader, "a_Texcoord");
	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);

	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float)*3));

	int pointUloc = glGetUniformLocation(shader, "u_Point");
	glUniform2f(pointUloc, 0.3, 0.3);
	int pointsUloc = glGetUniformLocation(shader, "u_Points");
	float points[] = { 0.1, 0.1, 0.5, 0.5, 0.8, 0.8 };
	glUniform2fv(pointsUloc, 3, points);
	int timeUloc = glGetUniformLocation(shader, "u_Time");
	glUniform1f(timeUloc, g_time);
	g_time += 0.004;

	//
	int uniformLoc_Texture = glGetUniformLocation(shader, "u_Texture");
	glUniform1f(uniformLoc_Texture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RGBTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::Class0310()
{
	float vertices[] = { 0, 0, 0, 1, 0, 0, 1, 1, 0 };

	glGenBuffers(1, &m_testVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, 
		GL_STATIC_DRAW);

	float vertices1[] = { -1, -1, 0, 
						0, -1, 0, 
						0, 0, 0 };

	glGenBuffers(1, &m_testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1,
		GL_STATIC_DRAW);

	float color[] = {  1, 0, 0, 1,
					   0, 1, 0, 1,
					   0, 0, 1, 1 };

	glGenBuffers(1, &m_ColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color,
		GL_STATIC_DRAW);

	/*int size = 40000000000000;
	float* testTemp = new float[size];
	memset(testTemp, 1, sizeof(float)*size);

	GLuint testVBO1 = 0;
	glGenBuffers(1, &testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*size,
		testTemp,
		GL_STATIC_DRAW);*/

}

void Renderer::CreateTexures()
{
	GLulong checkerboard[] =
	{
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF
	};


	glGenTextures(1, &m_CheckerBoardTexture);
	glBindTexture(GL_TEXTURE_2D, m_CheckerBoardTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::CreateParticleVBO(int numParticleCount)
{
	int vertexCount = 6;
	int particleCount = numParticleCount;
	int floatCount = 3;
	int totalFloatCount = particleCount
		* vertexCount
		* floatCount;
	int totalFloatCountSingle = particleCount
		* vertexCount
		* 1;
	int totalFloatCountFour = particleCount
		* vertexCount
		* 4;

	m_ParticleVertexCount = particleCount * vertexCount;

	float* vertices = NULL;
	vertices = new float[totalFloatCount];

	float particleSize = 0.01f;

	int index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float particelCenterX = 0.f; // 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		float particelCenterY = 0.f; // 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		vertices[index] = particelCenterX - particleSize; index++;
		vertices[index] = particelCenterY + particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX - particleSize; index++;
		vertices[index] = particelCenterY - particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX + particleSize; index++;
		vertices[index] = particelCenterY + particleSize; index++;
		vertices[index] = 0.f; index++;

		vertices[index] = particelCenterX + particleSize; index++;
		vertices[index] = particelCenterY + particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX - particleSize; index++;
		vertices[index] = particelCenterY - particleSize; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = particelCenterX + particleSize; index++;
		vertices[index] = particelCenterY - particleSize; index++;
		vertices[index] = 0.f; index++;
	}

	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCount,
		vertices, GL_STATIC_DRAW);
	delete[] vertices;

	float* verticesVel = NULL;
	verticesVel = new float[totalFloatCount];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float velX = 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		float velY = 4.f*(((float)rand() / (float)RAND_MAX));
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
		
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;;
		verticesVel[index] = velX; index++;
		verticesVel[index] = velY; index++;
		verticesVel[index] = 0.f; index++;
	}

	glGenBuffers(1, &m_ParticleVelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCount,
		verticesVel, GL_STATIC_DRAW);
	delete[] verticesVel;
	
	//EmitTime
	float* verticesEmitTime = NULL;
	verticesEmitTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float emitTime = 10.f*(((float)rand() / (float)RAND_MAX));
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;
		verticesEmitTime[index] = emitTime; index++;;
	}

	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesEmitTime, GL_STATIC_DRAW);
	delete[] verticesEmitTime;

	//LifeTime
	float* verticesLifeTime = NULL;
	verticesLifeTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float lifeTime = 1.f*(((float)rand() / (float)RAND_MAX));
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;
		verticesLifeTime[index] = lifeTime; index++;;
	}

	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesLifeTime, GL_STATIC_DRAW);
	delete[] verticesLifeTime;

	//period
	float* verticesPeriodTime = NULL;
	verticesPeriodTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float period = 1.f*(((float)rand() / (float)RAND_MAX));
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;
		verticesPeriodTime[index] = period; index++;;
	}

	glGenBuffers(1, &m_ParticlePeriodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesPeriodTime, GL_STATIC_DRAW);
	delete[] verticesPeriodTime;

	//amp
	float* verticesAmpTime = NULL;
	verticesAmpTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float amp = 1.f*(2.f*(((float)rand() / (float)RAND_MAX))-1.f);
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;
		verticesAmpTime[index] = amp; index++;;
	}

	glGenBuffers(1, &m_ParticleAmpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesAmpTime, GL_STATIC_DRAW);
	delete[] verticesAmpTime;

	//value
	float* verticesValue = NULL;
	verticesValue = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float value = 1.f*((float)rand() / (float)RAND_MAX);
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;
		verticesValue[index] = value; index++;;
	}

	glGenBuffers(1, &m_ParticleValueVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleValueVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*totalFloatCountSingle,
		verticesValue, GL_STATIC_DRAW);
	delete[] verticesValue;	

	//color
	float* verticesColor = NULL;
	verticesColor = new float[totalFloatCountFour];

	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float r = 1.f*((float)rand() / (float)RAND_MAX);
		float g = 1.f*((float)rand() / (float)RAND_MAX);
		float b = 1.f*((float)rand() / (float)RAND_MAX);
		float a = 1.f*((float)rand() / (float)RAND_MAX);

		verticesColor[index] = r; index++;
		verticesColor[index] = g; index++;
		verticesColor[index] = b; index++;
		verticesColor[index] = a; index++;//v1

		verticesColor[index] = r; index++;
		verticesColor[index] = g; index++;
		verticesColor[index] = b; index++;
		verticesColor[index] = a; index++;//v2

		verticesColor[index] = r; index++;
		verticesColor[index] = g; index++;
		verticesColor[index] = b; index++;
		verticesColor[index] = a; index++;//v3

		verticesColor[index] = r; index++;
		verticesColor[index] = g; index++;
		verticesColor[index] = b; index++;
		verticesColor[index] = a; index++;//v4

		verticesColor[index] = r; index++;
		verticesColor[index] = g; index++;
		verticesColor[index] = b; index++;
		verticesColor[index] = a; index++;//v5

		verticesColor[index] = r; index++;
		verticesColor[index] = g; index++;
		verticesColor[index] = b; index++;
		verticesColor[index] = a; index++;//v6
	}

	glGenBuffers(1, &m_ParticleColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleColorVBO);
	glBufferData(GL_ARRAY_BUFFER, 
		sizeof(float)*totalFloatCountFour,
		verticesColor, GL_STATIC_DRAW);
	delete[] verticesColor;
	   	  
	//pos+color+UV vbo
	int totalFloatCountPosColUV = numParticleCount * 6 *
		(3 + 4 + 2);
	float* verticesPosColorUV = NULL;
	verticesPosColorUV = new float[totalFloatCountPosColUV];
	
	index = 0;
	for (int i = 0; i < numParticleCount; i++)
	{
		float particelCenterX = 0.f; // 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		float particelCenterY = 0.f; // 2.f*(((float)rand() / (float)RAND_MAX) - 0.5f);
		float r = 100.f * (float)rand() / (float)RAND_MAX;
		float g = 100.f * (float)rand() / (float)RAND_MAX;
		float b = 100.f * (float)rand() / (float)RAND_MAX;
		float a = 1.f*((float)rand() / (float)RAND_MAX);

		verticesPosColorUV[index] = particelCenterX - particleSize; index++;
		verticesPosColorUV[index] = particelCenterY + particleSize; index++;
		verticesPosColorUV[index] = 0.f; index++; //xyz
		verticesPosColorUV[index] = r; index++;
		verticesPosColorUV[index] = g; index++;
		verticesPosColorUV[index] = b; index++;
		verticesPosColorUV[index] = a; index++; //rgba
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = 0.f; index++; //UV //v1

		verticesPosColorUV[index] = particelCenterX - particleSize; index++;
		verticesPosColorUV[index] = particelCenterY - particleSize; index++;
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = r; index++;
		verticesPosColorUV[index] = g; index++;
		verticesPosColorUV[index] = b; index++;
		verticesPosColorUV[index] = a; index++;
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = 1.f; index++; //UV //v2

		verticesPosColorUV[index] = particelCenterX + particleSize; index++;
		verticesPosColorUV[index] = particelCenterY + particleSize; index++;
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = r; index++;
		verticesPosColorUV[index] = g; index++;
		verticesPosColorUV[index] = b; index++;
		verticesPosColorUV[index] = a; index++;
		verticesPosColorUV[index] = 1.f; index++;
		verticesPosColorUV[index] = 0.f; index++; //UV //v3

		verticesPosColorUV[index] = particelCenterX + particleSize; index++;
		verticesPosColorUV[index] = particelCenterY + particleSize; index++;
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = r; index++;
		verticesPosColorUV[index] = g; index++;
		verticesPosColorUV[index] = b; index++;
		verticesPosColorUV[index] = a; index++;
		verticesPosColorUV[index] = 1.f; index++;
		verticesPosColorUV[index] = 0.f; index++; //UV //v4

		verticesPosColorUV[index] = particelCenterX - particleSize; index++;
		verticesPosColorUV[index] = particelCenterY - particleSize; index++;
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = r; index++;
		verticesPosColorUV[index] = g; index++;
		verticesPosColorUV[index] = b; index++;
		verticesPosColorUV[index] = a; index++;
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = 1.f; index++; //UV //v5

		verticesPosColorUV[index] = particelCenterX + particleSize; index++;
		verticesPosColorUV[index] = particelCenterY - particleSize; index++;
		verticesPosColorUV[index] = 0.f; index++;
		verticesPosColorUV[index] = r; index++;
		verticesPosColorUV[index] = g; index++;
		verticesPosColorUV[index] = b; index++;
		verticesPosColorUV[index] = a; index++;
		verticesPosColorUV[index] = 1.f; index++;
		verticesPosColorUV[index] = 1.f; index++; //UV //v6
	}

	glGenBuffers(1, &m_ParticlePosColUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColUVVBO);
	glBufferData(GL_ARRAY_BUFFER, 
		sizeof(float)*totalFloatCountPosColUV,
		verticesPosColorUV, GL_STATIC_DRAW);
	delete[] verticesPosColorUV;


//=========================================

}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)
{
	//Load Png
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);

	if (error != 0)
	{
		std::cout << "PNG image loading failed : " << filePath << std::endl;
		assert(0);
	}

	GLuint temp;
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

	return temp;
}

void Renderer::CreateGridMesh()
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;

	int pointCountX = 64;
	int pointCountY = 64;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	float* point = new float[pointCountX * pointCountY * 2];
	float* vertices = new float[(pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3];
	m_GridMeshVertexCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;

	//Prepare points
	for (int x = 0; x < pointCountX; x++)
	{
		for (int y = 0; y < pointCountY; y++)
		{
			point[(y * pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
			point[(y * pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; x++)
	{
		for (int y = 0; y < pointCountY - 1; y++)
		{
			//Triangle part 1
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;

			//Triangle part 2
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
		}
	}

	glGenBuffers(1, &m_GridMeshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3, vertices, GL_STATIC_DRAW);

}

void Renderer::CreateFBOs()
{
	glGenTextures(1, &m_AFBOTexture);
	glBindTexture(GL_TEXTURE_2D, m_AFBOTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); 

	glGenTextures(1, &m_AFBOAttach_1_Texture);
	glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_1_Texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glGenTextures(1, &m_AFBOAttach_2_Texture);
	glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_2_Texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glGenTextures(1, &m_AFBOAttach_3_Texture);
	glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_3_Texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glGenTextures(1, &m_AFBOAttach_4_Texture);
	glBindTexture(GL_TEXTURE_2D, m_AFBOAttach_4_Texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	 

	glGenTextures(1, &m_BFBOTexture);
	glBindTexture(GL_TEXTURE_2D, m_BFBOTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &m_CFBOTexture);
	glBindTexture(GL_TEXTURE_2D, m_CFBOTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &m_DFBOTexture);
	glBindTexture(GL_TEXTURE_2D, m_DFBOTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenRenderbuffers(1, &m_DepthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//glGenFramebuffers(1, &m_A_FBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, m_A_FBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AFBOTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_AFBOAttach_1_Texture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_AFBOAttach_2_Texture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_AFBOAttach_3_Texture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_AFBOAttach_4_Texture, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

	glGenFramebuffers(1, &m_A_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_A_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AFBOTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

	glGenFramebuffers(1, &m_B_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_B_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BFBOTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

	glGenFramebuffers(1, &m_C_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_C_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_CFBOTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

	glGenFramebuffers(1, &m_D_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_D_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_DFBOTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);



	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); // ���� üũ
	
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FBO creation failed" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawTexture(float x, float y, float scaleX, float scaleY, GLuint texID)
{
	GLuint shader = m_DrawTextureShader;
	glUseProgram(shader);

	GLuint posLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posLoc);
	GLuint texLoc = glGetAttribLocation(shader, "a_TexPos");
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_DrawTextureVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLuint samplerULoc = glGetUniformLocation(shader, "u_TexSampler");
	glUniform1i(samplerULoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	
	GLuint posScaleULoc = glGetUniformLocation(shader, "u_PosScale");
	glUniform4f(posScaleULoc, x, y, scaleX, scaleY);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::PrepareBloom()
{
	m_GaussianBlurHShader = CompileShaders("./Shaders/GaussianBlur.vs", "./Shaders/GaussianBlurH.fs");
	m_GaussianBlurVShader = CompileShaders("./Shaders/GaussianBlur.vs", "./Shaders/GaussianBlurV.fs");
	m_DrawMergeTextureShader = CompileShaders("./Shaders/MergeTexture.vs", "./Shaders/MergeTexture.fs");

	float sizeX = 1.f;
	float sizeY = 1.f;

	float fullRectTextureRect[] =
	{
		-sizeX, sizeY, 0.f,		0.f, 0.f,
		-sizeX, -sizeY, 0.f,	0.f, 1.f,
		sizeX, sizeY, 0.f,		1.f, 0.f,
		sizeX, sizeY, 0.f,		1.f, 0.f,
		-sizeX, -sizeY, 0.f,	0.f, 1.f,
		sizeX, -sizeY, 0.f,		1.f, 1.f,
	};
	glGenBuffers(1, &m_FullRectVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FullRectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullRectTextureRect), fullRectTextureRect, GL_STATIC_DRAW);

	// Gen HDR FBO
	glGenFramebuffers(1, &m_HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);

	//
	glGenTextures(1, &m_HDRHighTexture);
	glBindTexture(GL_TEXTURE_2D, m_HDRHighTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &m_HDRLowTexture);
	glBindTexture(GL_TEXTURE_2D, m_HDRLowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_HDRLowTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_HDRHighTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "fbo creation failed" << std::endl;

	// Gen Pingpong FBOs
	glGenFramebuffers(2, m_PingpongFBO);
	glGenTextures(2, m_PingpongTexture);

	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_PingpongTexture[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PingpongTexture[i], 0);
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "fbo creation failed" << std::endl;
	}	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

