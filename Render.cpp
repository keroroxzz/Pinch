#include "Render.h"

extern Shader *resetShader, *brushShader, *copyShader, *distortShader, *displayShader;

void DrawScreenRect(int width, int height)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0, 0.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0.0, height);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(width, height);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(width, 0.0);
	glEnd();
}

void SetFullRender(int width, int height)
{
	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, 0.0, 20.0);
}

void BindFBO(GLuint fbo, int width, int height)
{
	glFinish();

	SetFullRender(width, height);

	glPushAttrib(GL_VIEWPORT_BIT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	DrawScreenRect(width, height);

	glUseProgram(0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
}

void ResetFrame(GLuint fbo, int width, int height)
{
	resetShader->Use();
	BindFBO(fbo, width, height);
}

void RenderOutput( GLuint src, GLuint fbo, int width, int height)
{
	distortShader->Use();
	distortShader->SetUniform("sampler0", UNI_TEXTURE, NULL, src);
	distortShader->SetUniform("sampler1", UNI_TEXTURE, NULL, 1);

	BindFBO(fbo, width, height);
}

void prepareFBO(GLenum textureNum, GLuint *fbo, GLuint *fboTexture, int width, int height, GLint internalformat, GLenum format, GLenum type)
{
	glActiveTexture(textureNum);
	glBindTexture(GL_TEXTURE_2D, *fboTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//generate frame buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, *fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, *fboTexture, 0);

	//check the status of the frame buffer
	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		printf("Could not validate framebuffer");
	}
	//reset frame buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	printf("Generate texture %d and fbo %d.\n", *fboTexture, *fbo);
}