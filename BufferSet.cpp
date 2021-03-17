#include "BufferSet.h"

extern Shader *resetShader, *brushShader, *copyShader, *distortShader, *displayShader;

BufferSet::~BufferSet()
{
	glDeleteTextures(size, texture);
	glDeleteFramebuffersEXT(1, &fbo);

	delete[] texture;
}

BufferSet::BufferSet(int size, GLenum textureNumBegin) : size(size), texSize{0}
{
	index = 0;
	texture = new GLuint[size];
	texNumBegin = textureNumBegin - GL_TEXTURE0;

	for (int i = 0; i < size; i++)
	{
		glActiveTexture(textureNumBegin + i);
		glGenTextures(1, &texture[i]);
	}

	glGenFramebuffersEXT(1, &fbo);
}

int BufferSet::GetCurrentIndex()
{
	return index;
}

int BufferSet::GetCurrentTexNum()
{
	return index + texNumBegin;
}

int BufferSet::PrevIndex()
{
	return (index == 0 ? size : index) - 1;
}

int BufferSet::PrevTexNum()
{
	return texNumBegin + (index == 0 ? size : index) - 1;
}

void BufferSet::UpdateFrameBuffer()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture[index], 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void BufferSet::Next(bool clean)
{
	index = (index + 1) % size;
	UpdateFrameBuffer();
	if(clean)
		CopyFrom(PrevTexNum());
}

void BufferSet::Prev()
{
	index = (index == 0 ? size : index) - 1;
	UpdateFrameBuffer();
}

void BufferSet::CopyFrom(GLuint texNum)
{
	glFinish();

	copyShader->Use();
	copyShader->SetUniform("sampler0", UNI_TEXTURE, NULL, texNum);

	BindFBO(fbo, texSize[0], texSize[1]);

	glUseProgram(0);
}

void BufferSet::Reset(int width, int height)
{
	texSize[0] = width;
	texSize[1] = height;

	for (int i = 0; i < size; i++)
	{
		prepareFBO(GL_TEXTURE0 + texNumBegin + i, &fbo, &texture[i], texSize[0], texSize[1], GL_RGBA16, GL_RGBA, GL_FLOAT);
		ResetFrame(fbo, texSize[0], texSize[1]);
	}

	index = 0;

	UpdateFrameBuffer();
}
