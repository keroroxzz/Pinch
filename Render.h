#pragma once

#include "baseHeader.h"


void DrawScreenRect(int width, int height);
void SetFullRender(int width, int height);
void ResetFrame(GLuint fbo, int width, int height);
void RenderOutput(GLuint src, GLuint fbo, int width, int height);
void BindFBO(GLuint fbo, int width, int height);

void prepareFBO(GLenum textureNum, GLuint *fbo, GLuint *fboTexture, int width, int height, GLint internalformat, GLenum format, GLenum type);

