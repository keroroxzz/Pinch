/*
BufferSet
This class manages the textures of OpenGL as step recording buffers.

By Brian Tu
Data : 2021/3/17
*/

#ifndef BUFFERSET_HEADER
#define BUFFERSET_HEADER

#include "baseHeader.h"

class BufferSet
{
	const int size;
	int index, texNumBegin;
	GLuint* texture, fbo;
	float texSize[2];

private:
	BufferSet() :size(0) {};

	void UpdateFrameBuffer();

public:
	BufferSet(int size, GLenum textureNumBegin);
	~BufferSet();

	int GetCurrentIndex();
	int GetCurrentTexNum();
	int PrevIndex();
	int PrevTexNum();

	void Next(bool clean = true);
	void Prev();

	void CopyFrom(GLuint texNum);
	void Reset(int width, int height);

};


#endif // !BUFFERSET_HEADER