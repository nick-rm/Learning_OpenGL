#pragma once

//Class: CShader
//Wraps OpenGL shader loading and compiling.

class ShaderClass
{
public:
	bool loadShader(char * sFile, int a_iType);
	void deleteShader();

	bool isLoaded();
	UINT getShaderID();

	ShaderClass();

private:
	UINT uiShader; // ID of shader
	int iType; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool bLoaded; // Whether shader was loaded and compiled
};

//Class: CShaderProgram
//Wraps OpenGL shader program

class ShaderProgClass
{
public:
	void createProgram();
	void deleteProgram();

	bool addShaderToProgram(ShaderClass* sShader);
	bool linkProgram();

	void useProgram();

	UINT getProgramID();

	ShaderProgClass();

private:
	UINT uiProgram; // ID of program
	bool bLinked; // Whether program was linked and is ready to use
};