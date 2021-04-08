/*
Shader files in char array
by Brian Tu
Date : 2021/4/7

Okay, it's a stupid way, and the only one I know to prevent extra shader file.
*/

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
"	in vec2 TexCoord;"
"	out vec4 FragColor;"
"   uniform sampler2D sampler0;"
"	void main(void){FragColor = texture(sampler0, TexCoord);}"
};

char reset_shader[] =
{
	"#version 330 core \r\n"
"	in vec2 TexCoord;"
"	out vec4 FragColor;"
"	void main(void){FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 0.0);}"
};

char display_shader[] =
{
"#version 330 core \r\n"
"in vec2 TexCoord;"

"out vec4 FragColor;"

"uniform vec3 brushIn;"
"uniform vec4 mouseIn;"
"uniform vec2 textureSize;"


"uniform sampler2D sampler0;"
"uniform sampler2D sampler1;"

"void main()"
"{"
"  int x = int(TexCoord.x * 50.0);"
    "  int y = int(TexCoord.y * 50.0);"
    "  vec3 checkboard = vec3((x + y) % 2) * 0.25 + 0.5;"
    "  float r = length(TexCoord.xy * textureSize - mouseIn.xy);"
    "  vec4 brush = vec4(1.0, 0.0, 0.0, 0.5 * smoothstep(brushIn.y, brushIn.z, r));"
    "  vec2 displacement = texture(sampler0, TexCoord).xy;"
    "  vec4 sampling = texture(sampler1, displacement).xyzw;"
    "  vec4 r0 = vec4(checkboard, 1.0);"
    "  r0 = r0 * (1.0 - sampling.w) + sampling * sampling.w;"
    "  r0 = r0 * (1.0 - brush.w) + brush * brush.w;"
    "  float ratio = 0.75 - abs(brushIn.y - r);"
    "  ratio = (ratio < 0.0 ? 0.0 : ratio) * 0.5;"
    "  FragColor = vec4(vec3(ratio) + r0.xyz * (1.0 - 2.0 * ratio), 1.0);"
"}"
};

char distort_shader[] =
{
"#version 330 core \r\n"
"in vec2 TexCoord;"
"out vec4 FragColor;"
"uniform sampler2D sampler0;"
"uniform sampler2D sampler1;"
"void main()"
"{"
"  vec2 displacement = texture(sampler0, TexCoord).xy;"
    "  vec4 sampling = texture(sampler1, displacement).xyzw;"
    "  FragColor = sampling.zyxw;"
"}"
};

char brush_shader[] =
{
"#version 330 core \r\n"
"in vec2 TexCoord;"
"out vec4 FragColor;"
"uniform vec3 brush;"
"uniform vec4 mouseIn;"
"uniform vec2 textureSize;"
"uniform float step;"
"uniform sampler2D sampler0;"
"void main()"
"{"
"  vec2 m = mouseIn.xy;"
    "  vec2 d = mouseIn.zw / step;"
    "    float s = 0.0;"
    "    for (float i = 0; i < step; i++)"
        "    {"
        "      float r = length(gl_FragCoord.xy - m);"
        "      s += smoothstep(brush.y, brush.z, r) * brush.x;"
        "      m += d;"
        "    }"
        "    FragColor = texture(sampler0, TexCoord + d * s / textureSize);"
"}"
};