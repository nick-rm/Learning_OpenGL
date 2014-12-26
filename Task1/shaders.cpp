#include <glew.h>
#include <wglew.h>
#include <stdio.h>
#include <stdlib.h>

#include "shaders.h"

ShaderClass::ShaderClass()
{
	bLoaded = false;
}

bool ShaderClass::loadShader(char * sFile, int a_iType)
{
	FILE* fp = fopen(sFile, "rt");
	if(!fp)return false;

	//Loading shader from a file into an array

	GLchar * strings[128];
	int stringsI=0;

	char sLine[255];
	while(fgets(sLine, 255, fp))
	{
		strings[stringsI]=(GLchar*)malloc(255);
		strcpy(strings[stringsI],sLine); 
		stringsI++;
	}
	fclose(fp);

	uiShader = glCreateShader(a_iType);

	glShaderSource(uiShader, stringsI, (const GLchar**)strings, NULL);
	glCompileShader(uiShader);

	int iCompilationStatus;
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)return false;
	iType = a_iType;
	bLoaded = true;

	return 1;
}

bool ShaderClass::isLoaded()
{
	return bLoaded;
}

UINT ShaderClass::getShaderID()
{
	return uiShader;
}

void ShaderClass::deleteShader()
{
	if(!isLoaded())return;
	bLoaded = false;
	glDeleteShader(uiShader);
}

ShaderProgClass::ShaderProgClass()
{
	bLinked = false;
}

void ShaderProgClass::createProgram()
{
	uiProgram = glCreateProgram();
}

bool ShaderProgClass::addShaderToProgram(ShaderClass* sShader)
{
	if(!sShader->isLoaded())return false;

	glAttachShader(uiProgram, sShader->getShaderID());

	return true;
}

bool ShaderProgClass::linkProgram()
{
	glLinkProgram(uiProgram);
	int iLinkStatus;
	glGetProgramiv(uiProgram, GL_LINK_STATUS, &iLinkStatus);
	bLinked = iLinkStatus == GL_TRUE;
	return bLinked;
}

void ShaderProgClass::deleteProgram()
{
	if(!bLinked)return;
	bLinked = false;
	glDeleteProgram(uiProgram);
}

void ShaderProgClass::useProgram()
{
	if(bLinked)glUseProgram(uiProgram);
}

UINT ShaderProgClass::getProgramID()
{
	return uiProgram;
}