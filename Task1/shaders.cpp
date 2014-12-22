#include <glew.h>
#include <wglew.h>
#include <stdio.h>
#include <stdlib.h>

#include "shaders.h"

CShader::CShader()
{
	bLoaded = false;
}

bool CShader::loadShader(char * sFile, int a_iType)
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

bool CShader::isLoaded()
{
	return bLoaded;
}

UINT CShader::getShaderID()
{
	return uiShader;
}

void CShader::deleteShader()
{
	if(!isLoaded())return;
	bLoaded = false;
	glDeleteShader(uiShader);
}

CShaderProgram::CShaderProgram()
{
	bLinked = false;
}

void CShaderProgram::createProgram()
{
	uiProgram = glCreateProgram();
}

bool CShaderProgram::addShaderToProgram(CShader* shShader)
{
	if(!shShader->isLoaded())return false;

	glAttachShader(uiProgram, shShader->getShaderID());

	return true;
}

bool CShaderProgram::linkProgram()
{
	glLinkProgram(uiProgram);
	int iLinkStatus;
	glGetProgramiv(uiProgram, GL_LINK_STATUS, &iLinkStatus);
	bLinked = iLinkStatus == GL_TRUE;
	return bLinked;
}

void CShaderProgram::deleteProgram()
{
	if(!bLinked)return;
	bLinked = false;
	glDeleteProgram(uiProgram);
}

void CShaderProgram::useProgram()
{
	if(bLinked)glUseProgram(uiProgram);
}

UINT CShaderProgram::getProgramID()
{
	return uiProgram;
}