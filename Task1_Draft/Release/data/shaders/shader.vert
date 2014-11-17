#version 330

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProjection;

layout (location = 0) in vec3 inPosition1;
layout (location = 1) in vec3 inPositionDiffer;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTex;

smooth out vec3 theColor;

uniform float time;


void main()
{
	gl_Position = mProjection*mView*mWorld*vec4((inPosition1 + inPositionDiffer * time), 1.0);
	vec3 pos= vec3(gl_Position)*0.5;
	vec3 ligt=vec3(0.25,0.0,1.0f);
	vec3 color=dot(ligt,pos) * (vec3(0.75+inTex.x,0.75+inTex.y+inTex.x,0.75+inTex.y)) *0.3f;
	theColor = color;
}