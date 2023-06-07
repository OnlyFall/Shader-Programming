#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"

#define TEST_MODE 1

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void Class0310_Rendering();
	void DrawParticle();
	void DrawFragmentSandbox();
	void DrawAlphaClear();
	void DrawVertexSandbox();
	void DrawTextureSandbox();
	void DrawVSFrag();
	void DrawGridMesh();

	void DrawTest();

	void DrawResult();

	void DrawParticleWithBloom();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GetGLPosition(float x, float y, float *newX, float *newY);
	void Class0310();
	
	void CreateTexures();
	GLuint CreatePngTexture(char* filePath, GLuint samplingMethod);
	void CreateGridMesh();
	void CreateFBOs();
	void PrepareBloom();

	void CreateParticleVBO(int numParticleCount);
	GLuint m_ParticleShader = -1;
	GLuint m_ParticleVBO = -1;
	GLuint m_ParticleVelVBO = -1;
	GLuint m_ParticleEmitTimeVBO = -1;
	GLuint m_ParticleLifeTimeVBO = -1;
	GLuint m_ParticlePeriodVBO = -1;
	GLuint m_ParticleAmpVBO = -1;
	GLuint m_ParticleValueVBO = -1;
	GLuint m_ParticleColorVBO = -1;
	GLuint m_ParticlePosColUVVBO = -1;
	GLuint m_ParticleVertexCount = -1;
	GLuint m_ParticlePositionColorVelUVVBO = -1;
	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_testVBO = 0;
	GLuint m_testVBO1 = 0;
	GLuint m_ColorVBO = 0;

	//for fragment sandbox
	GLuint m_FragmentSandboxShader = 0;
	GLuint m_FragmentSandboxVBO = 0;

	//for trail
	GLuint m_AlphaClearShader = 0;
	GLuint m_AlphaClearVBO = 0;

	//for vertex anim
	GLuint m_VertexSandboxShader = 0;
	GLuint m_HorizontalLineVBO = 0;
	GLuint m_HorizontalLineVertexCount = 0;

	GLuint m_TextureSandboxShader = 0;
	GLuint m_TextureSandboxVBO = 0;
	GLuint m_CheckerBoardTexture = 0;
	GLuint m_RGBTexture = 0;

	// 텍스쳐 6개

	GLuint m_Texture[6]{};

	GLuint m_0Texture = 0;
	GLuint m_1Texture = 0;
	GLuint m_2Texture = 0;
	GLuint m_3Texture = 0;
	GLuint m_4Texture = 0;
	GLuint m_5Texture = 0;
	GLuint m_MergeTexture = 0;

	// 시험 연습용
	GLuint m_testShader = 0;
	GLuint m_practiceVBO = -1;

	GLuint m_ParticleTexture = 0;
	GLuint m_ExplosionTexture = 0;

	GLuint m_GridMeshShader = 0;
	GLuint m_GridMeshVBO = 0;
	GLuint m_GridMeshVertexCount = 0;

	// 2023-05-24 온라인강의 FBO
	

	GLuint m_AFBOAttach_1_Texture = 0;
	GLuint m_AFBOAttach_2_Texture = 0;
	GLuint m_AFBOAttach_3_Texture = 0;
	GLuint m_AFBOAttach_4_Texture = 0;

	GLuint m_AFBOTexture = 0;
	GLuint m_BFBOTexture = 0;
	GLuint m_CFBOTexture = 0;
	GLuint m_DFBOTexture = 0;
	GLuint m_DepthRenderBuffer = 0;

	GLuint m_A_FBO = 0;
	GLuint m_B_FBO = 0;
	GLuint m_C_FBO = 0;
	GLuint m_D_FBO = 0;

	// 2023-05-26 추가 이미지
	GLuint m_rabbit_texture = 0;

	// 2023-06-01 추가 DrawTexture
	void DrawTexture(float x, float y, float scaleX, float scaleY, GLuint texID);
	GLuint m_DrawTextureVBO = 0;
	GLuint m_DrawTextureShader = 0;

	// 2023-06-06 추가 Bloom
	GLuint m_HDRFBO = 0;
	GLuint m_HDRLowTexture = 0;
	GLuint m_HDRHighTexture = 0;
	GLuint m_PingpongFBO[2] = { 0,0 };
	GLuint m_PingpongTexture[2] = { 0,0 };
	GLuint m_FullRectVBO = 0;
	GLuint m_GaussianBlurHShader = 0;
	GLuint m_GaussianBlurVShader = 0;

	void DrawGaussianBlur(GLuint texID, GLuint targetFBOID, GLuint shader);

	GLuint m_DrawMergeTextureShader = 0;

	void DrawMergeBloomTexture(GLuint sceneTexID, GLuint bloomTexID, float exposure);
};

