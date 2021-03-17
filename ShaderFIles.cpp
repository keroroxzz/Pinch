char basic_shader[] =
{
	"#version 330 core \r\n"
"	layout(location = 0) in vec3 aPos; \r\n"
"	layout(location = 8) in vec2 aTex; \r\n"
"	uniform mat4 gl_ModelViewProjectionMatrix; \r\n"
"	out vec2 TexCoord; \r\n"
"	void main(void){ \r\n"
"   #if __VERSION__ >= 140   \r\n"
"   TexCoord = aTex.st; \r\n"
"   #else \r\n"
"   TexCoord = gl_MultiTexCoord0.st; \r\n"
"   #endif \r\n"
"   gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos, 1.0);}"
};

char copy_shader[] =
{
	"#version 330 core \r\n"
"	in vec2 TexCoord; \r\n"
"	out vec4 FragColor; \r\n"
"	void main(void){FragColor = texture(sampler0, TexCoord);}"
};

char reset_shader[] =
{
	"#version 330 core \r\n"
"	in vec2 TexCoord; \r\n"
"	out vec4 FragColor; \r\n"
"	void main(void){FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 0.0);}"
};