/*
    Flow by Brian Tu
    Version : Alpha 1.0
    Date : 2021/3/10
*/

#include "baseHeader.h"
#include "BufferSet.h"

extern char basic_shader[];
extern char copy_shader[];
extern char reset_shader[];

Shader *resetShader, * brushShader, *copyShader, *distortShader, *displayShader;

GLuint fbo_tmp, tmp_texture, in_texture, fbo_out, out_texture;

GLint windowWidth = 600;               // window size
GLint windowHeight = 600;

GLint vpWidth = 600;               // viewport size
GLint vpHeight = 600;

GLint textureWidth;
GLint textureHeight;
float textureSize[2];

BITMAPV5HEADER* header;
BYTE* follower;
size_t headerSize;

float mouseScreen[5], mouseObj[5];	//Last element is not used, just for the convinece of displacement caculation
GLfloat mouseStep;
float brushProfile[3] = {0.0, 100.0, 0.0};	//pressure, outter radius, inner radius

GLint mvp[4];
double mdv[16], mpj[16];
float usm[9];	//un scaling matrix
float upm[16];	//un projecting matrix

int mouseButtonStates[16]{ 0 };
bool keyboard[256]{ 0 };

BufferSet *buffers;

// Called to draw scene
void RenderScene(void)
{
    //reset frame
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	SetFullRender(textureWidth, textureHeight);

    if (brushProfile[0] > 0.0)
    {
        //apply and set up the brush shader
        brushShader->Use();
        brushShader->SetUniform("brush", UNI_VEC_3, &brushProfile);
        brushShader->SetUniform("mouseIn", UNI_VEC_4, &mouseObj);
		brushShader->SetUniform("textureSize", UNI_VEC_2, &textureSize);
        brushShader->SetUniform("step", UNI_FLOAT_1, &mouseStep);

        //bind the brush texture for reading
        brushShader->SetUniform("sampler0", UNI_TEXTURE, NULL, buffers->GetCurrentTexNum());

        BindFBO(fbo_tmp ,textureWidth, textureHeight);

		buffers->CopyFrom(0);
    }

	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, vpWidth, 0.0, vpHeight, 0.0, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPushMatrix();

	glGetDoublev(GL_MODELVIEW_MATRIX, mdv);
	glGetDoublev(GL_PROJECTION_MATRIX, mpj);
	glGetIntegerv(GL_VIEWPORT, mvp);

	displayShader->Use();

    displayShader->SetUniform("brushIn", UNI_VEC_3, &brushProfile);
	displayShader->SetUniform("mouseIn", UNI_VEC_4, &mouseObj);
	displayShader->SetUniform("textureSize", UNI_VEC_2, &textureSize);
	displayShader->SetUniform("sampler0", UNI_TEXTURE, NULL, buffers->GetCurrentTexNum());
	displayShader->SetUniform("sampler1", UNI_TEXTURE, NULL, 1);

	DrawScreenRect(textureWidth, textureHeight);

    glUseProgram(0);

    glutSwapBuffers();
}

void LoadClipboard(GLint &width, GLint &height)
{
	HWND hwnd = GetClipboardOwner();
	if (!OpenClipboard(hwnd))
	{
		CloseClipboard();
		printf("Failed to open the clipboard!\n");
		return;
	}

	HGLOBAL hglb = GetClipboardData(17);
	BITMAPV5HEADER* bv5header = (BITMAPV5HEADER*)GlobalLock(hglb);

	if(bv5header == nullptr)
	{
		GlobalUnlock(hglb);
		CloseClipboard();
		printf("Failed to get the header!\n");
		return;
	}

	//save the size of copied data
	headerSize = bv5header->bV5Size + bv5header->bV5ClrUsed * sizeof(RGBQUAD);

	BYTE* bmpLoaction = (BYTE*)bv5header;

	//copy the header
	header = (BITMAPV5HEADER*)malloc(sizeof(BITMAPV5HEADER));
	memcpy(header, bmpLoaction, sizeof(BITMAPV5HEADER));
	bmpLoaction += sizeof(BITMAPV5HEADER);

	//copy the following part, which is the color table
	follower = (BYTE*)malloc(bv5header->bV5ClrUsed * sizeof(RGBQUAD));
	memcpy(follower, bmpLoaction, bv5header->bV5ClrUsed * sizeof(RGBQUAD));

	bmpLoaction += bv5header->bV5ClrUsed * sizeof(RGBQUAD);
	bmpLoaction += bv5header->bV5Compression == BI_BITFIELDS ? 12 : 0;	//Bit Fields 

	//copy the image
	width = bv5header->bV5Width;
	height = bv5header->bV5Height;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bv5header->bV5Width, bv5header->bV5Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)bmpLoaction);
	
	if (GlobalUnlock(hglb) && CloseClipboard())
		printf("Clipboard image loaded..\tWidth:%d   Height%d   Compression:%d   BitCount:%d\n", bv5header->bV5Width, bv5header->bV5Height, bv5header->bV5Compression, bv5header->bV5BitCount);
	else
		printf("Failed to close the clipboard!\n");
}

void Pixele2Clipboard()
{
	HWND hwnd = GetClipboardOwner();
	if (!OpenClipboard(hwnd))
	{
		CloseClipboard();
		printf("Failed to open the clipboard!\n");
		return;
	}
	EmptyClipboard();

	HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, headerSize + textureWidth*textureHeight*sizeof(RGBQUAD));
	if (hglb == NULL)
	{
		CloseClipboard();
		printf("Failed to alloc memory!\n");
		return;
	}
	
	BYTE* buffer = (BYTE*)GlobalLock(hglb);

	if (buffer == nullptr)
	{
		GlobalUnlock(hglb);
		CloseClipboard();
		printf("Failed to get the header!\n");
		return;
	}

	memcpy(buffer, header, header->bV5Size);
	memcpy(buffer + header->bV5Size, follower, header->bV5ClrUsed * sizeof(RGBQUAD));

	buffer += header->bV5Size + header->bV5ClrUsed * sizeof(RGBQUAD);
	buffer += header->bV5Compression == BI_BITFIELDS ? 12 : 0;	//Bit Fields 

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_out);
	glReadPixels(0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	GlobalUnlock(hglb);

	SetClipboardData(17, hglb);
	CloseClipboard();
	printf("Copy the data to the clipboard!\n");
}

void InitializeTextures()
{
	//initialize the target texture
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, in_texture);
	LoadClipboard(textureWidth, textureHeight);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Set up the texture size
	textureSize[0] = textureWidth;
	textureSize[1] = textureHeight;

	//initialize the fbo and textures of tmp layer and output layer
	prepareFBO(GL_TEXTURE0 + 0, &fbo_tmp, &tmp_texture, textureWidth, textureHeight, GL_RGBA16, GL_RGBA, GL_FLOAT);
	prepareFBO(GL_TEXTURE0 + 2, &fbo_out, &out_texture, textureWidth, textureHeight, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

	ResetFrame(fbo_tmp, textureWidth, textureHeight);

	buffers->Reset(textureWidth, textureHeight);

	glutPostRedisplay();
}

void ReloadShaders()
{
	delete resetShader;
	delete brushShader;
	delete copyShader;
	delete distortShader;
	delete displayShader;

	resetShader = new Shader(2);
	//resetShader->AddFromString(basic_shader, GL_VERTEX_SHADER);
	resetShader->AddFromFile("shaders/vertex/basic.vs", GL_VERTEX_SHADER);
	resetShader->AddFromFile("shaders/frag/reset.fs", GL_FRAGMENT_SHADER);

	brushShader = new Shader(2);
	brushShader->AddFromFile("shaders/vertex/basic.vs", GL_VERTEX_SHADER);
	brushShader->AddFromFile("shaders/frag/brush.fs", GL_FRAGMENT_SHADER);

	copyShader = new Shader(2);
	copyShader->AddFromFile("shaders/vertex/basic.vs", GL_VERTEX_SHADER);
	copyShader->AddFromFile("shaders/frag/copy.fs", GL_FRAGMENT_SHADER);

	distortShader = new Shader(2);
	distortShader->AddFromFile("shaders/vertex/basic.vs", GL_VERTEX_SHADER);
	distortShader->AddFromFile("shaders/frag/distort.fs", GL_FRAGMENT_SHADER);

	displayShader = new Shader(2);
	displayShader->AddFromFile("shaders/vertex/basic.vs", GL_VERTEX_SHADER);
	displayShader->AddFromFile("shaders/frag/display.fs", GL_FRAGMENT_SHADER);
}

//initialization
void SetupRC()
{
	printf("Startup the program...\n");
	brushProfile[0] = -1.0;

    // Make sure required functionality is available!
    if (!GLEE_VERSION_2_0 && (!GLEE_ARB_fragment_shader ||
        !GLEE_ARB_shader_objects ||
        !GLEE_ARB_shading_language_100))
    {
        fprintf(stderr, "GLSL extensions not available!\n");
        return;
    }

    // Make sure we have multitexture and cube maps!
    if (!GLEE_VERSION_1_3 && (!GLEE_ARB_multitexture ||
        !GLEE_ARB_texture_cube_map))
    {
        fprintf(stderr, "Neither OpenGL 1.3 nor necessary"
            " extensions are available!\n");
        return;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Misc. state
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();
	glPushMatrix();

    GLbyte* tga;
    GLint width, height, component;
    GLenum format;

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    //Load Image
	glActiveTexture(GL_TEXTURE0 + 0);
	glGenTextures(1, &tmp_texture);

	glActiveTexture(GL_TEXTURE0 + 1);
	glGenTextures(1, &in_texture);

	glActiveTexture(GL_TEXTURE0 + 2);
	glGenTextures(1, &out_texture);
	

	glGenFramebuffersEXT(1, &fbo_tmp);
	glGenFramebuffersEXT(1, &fbo_out);

	buffers = new BufferSet(12, GL_TEXTURE0 + 3);

    glEnable(GL_TEXTURE_2D);

	ReloadShaders();
}

void ChangeSize(int w, int h)
{
	float 
		waspect = (float)w / h,
		taspect = (float)textureWidth / textureHeight;

	if (waspect < taspect)
	{
		vpWidth = textureWidth;
		vpHeight = textureWidth / waspect;
	}
	else
	{
		vpWidth = textureHeight * waspect;
		vpHeight = textureHeight;
	}

    windowWidth = w;
    windowHeight = h;

	glutPostRedisplay();
}

void UnProject(GLfloat x0, GLfloat y0,GLfloat *x, GLfloat *y)
{
	GLdouble mx, my, mz;
	gluUnProject(x0, windowHeight - y0, 1.0, mdv, mpj, mvp, &mx, &my, &mz);
	*x = mx;
	*y = my;
}

void UpdateMouse(int x, int y, bool updateObj=true)
{
	mouseScreen[2] = mouseScreen[0] - x;
	mouseScreen[3] = mouseScreen[1] - y;
	mouseScreen[0] = x;
	mouseScreen[1] = y;

	if (updateObj)
	{
		UnProject(mouseScreen[0], mouseScreen[1], &mouseObj[0], &mouseObj[1]);

		//Un-scaling matrix
		mouseObj[2] = mouseScreen[2] / mdv[0] / mpj[0] / mvp[2] * 2;
		mouseObj[3] = -mouseScreen[3] / mdv[5] / mpj[5] / mvp[3] * 2;
	}
}

#define MIN_STEP 5.0
void mouse(int x_, int y_)
{
	UpdateMouse(x_, y_, mouseButtonStates[1] != 1);
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	if (mouseButtonStates[0] == 1)	//left button pressed
	{
		brushProfile[0] = 1.0;

		float stepLength = sqrt(mouseObj[2] * mouseObj[2] + mouseObj[3] * mouseObj[3]);
		if (stepLength > brushProfile[1]/ MIN_STEP)
			mouseStep = ceil(stepLength / brushProfile[1] * MIN_STEP);
		else
			mouseStep = 1.0;
	}
	else if (mouseButtonStates[2] == 1)	//right button pressed
	{
		brushProfile[0] = 0.0;

		if (mouseObj[2] != 0.0 || mouseObj[3] != 0.0)
			glTranslatef(-mouseObj[2], -mouseObj[3], 0.0);
	}
	else if (mouseButtonStates[1] == 1) //wheel pressed
	{
		float angle = atan(abs(mouseScreen[3] / mouseScreen[2]))*180.0 / 3.14159,
			scale = 10.0/sqrt(windowWidth*windowWidth + windowHeight * windowHeight);

		if (angle < 20)
		{
			brushProfile[1] *= 1.0 - mouseScreen[2] * scale;
			brushProfile[2] *= 1.0 - mouseScreen[2] * scale;
		}
		else if (angle > 70)
		{\
			brushProfile[2] += mouseScreen[3] * scale * 100.0;
			brushProfile[2] = min(max(brushProfile[2], 0), brushProfile[1] * 0.999);
		}
	}

	glPushMatrix();

    glutPostRedisplay();
}

void mouseReleased(int x_, int y_)
{
	brushProfile[0] = 0.0;
	UpdateMouse(x_, y_);

    glutPostRedisplay();
}

void mouseState(int button, int state, int x, int y)
{
	if (button == 0 && mouseButtonStates[0] == 0 && state == 0)
	{
		buffers->Next();
		glutPostRedisplay();
	}

	if (button == 3 || button == 4)
	{
		float s = button == 3 ? 1.1 : 0.9;

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		UpdateMouse(x, y);
		glTranslated(mouseObj[0], mouseObj[1], 0.0);
		glScalef(s, s, 1.0);
		glTranslated(-mouseObj[0], -mouseObj[1], 0.0);

		glPushMatrix();
	}

	mouseButtonStates[button] = 1 - state;
    glutPostRedisplay();

#ifdef SHOW_KEYS
	printf("Mouse State, Button:%d  State:%d\n", button, state);
#endif // SHOW_KEYS
}

void KeyPressed(unsigned char key, int a, int b)
{
	keyboard[key] = true;

	if (keyboard[26])
	{
		buffers->Prev();
		glutPostRedisplay();
	}
	else if (keyboard[25])
	{
		buffers->Next(false);
		glutPostRedisplay();
	}
	else if (keyboard[3])
	{
		RenderOutput(buffers->GetCurrentTexNum(), fbo_out, textureWidth, textureHeight);
		Pixele2Clipboard();
	}
	else if (keyboard[22])
	{
		InitializeTextures();
		ChangeSize(windowWidth, windowHeight);
	}

#ifdef SHOW_KEYS
	printf("Pressed Key : %d\n", (int)key);
#endif // SHOW_KEYS
}

void KeyReleased(unsigned char key, int a, int b)
{
	keyboard[key] = false;

#ifdef SHOW_KEYS
		printf("Released Key : %d\n", (int)key);
#endif // SHOW_KEYS
}

int main(int argc, char* argv[])
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Pinch");

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutPassiveMotionFunc(mouseReleased);
    glutMotionFunc(mouse);
    glutMouseFunc(mouseState);

	glutKeyboardFunc(KeyPressed);
	glutKeyboardUpFunc(KeyReleased);

    SetupRC();


    glutMainLoop();

    return 0;
}
