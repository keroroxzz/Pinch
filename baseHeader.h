#pragma once


#ifndef BASEHEADER
#define BASEHEADER

//use free glut
#define FREEGLUT_STATIC

#include <iostream>
#include <fstream>
#include <math.h>
#include <windows.h>
#include <WinUser.h>

//opengl header files
#include <gl/shared/glee.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/shared/glut.h>

//my headers
#include "Shader.h"
#include "Render.h"

//libs
#pragma comment(lib,"legacy_stdio_definitions.lib")
#pragma comment(lib,"GLee.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glaux.lib")
#pragma comment(lib,"glut32.lib")

#endif // !BASEHEADER