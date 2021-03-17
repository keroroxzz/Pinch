#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 8) in vec2 aTex;

uniform mat4 gl_ModelViewProjectionMatrix;

out vec2 TexCoord;

void main(void)  
{  
   #if __VERSION__ >= 140  
   
   TexCoord = aTex.st;
   
   #else
   
   TexCoord = gl_MultiTexCoord0.st;
   
   #endif
	
   gl_Position = gl_ModelViewProjectionMatrix*vec4(aPos, 1.0);
}