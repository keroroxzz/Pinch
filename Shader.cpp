/*
    Shader Class 
    by Brian Tu
    Date : 2021/3/17
    Reference from OpenGL SuperBible, 4rth Edition, Richard Wright.
*/

#include "baseHeader.h"
#include "Shader.h"

#define MAX_INFO_SIZE 2048

//Constructor
Shader::Shader(int max_num) : maxShaders(max_num), currentIndex(0), validation(GL_FALSE)
{
    shaders = new GLint[maxShaders];

    //init shader program
    program = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(program);
    for (int i = 0; i < currentIndex; i++)
        glDeleteShader(shaders[i]);

    delete[] shaders;
}

bool Shader::AddFromString(const char* shader, int type)
{
    if (currentIndex >= maxShaders)
        return 0;

    CompileShader(shader, currentIndex, type);
    AttachShader(currentIndex);
    Validate();

    currentIndex++;

    return 1;
}

bool Shader::AddFromFile(const char* filename, int type)
{
    if (currentIndex >= maxShaders)
        return 0;

    char *shader;
    LoadShaderText(filename, &shader);
    CompileShader(shader, currentIndex, type);
    AttachShader(currentIndex);
    Validate();
    delete[] shader;

    currentIndex++;

    return 1;
}

void Shader::Use()
{
    glUseProgram(program);
}

bool Shader::LoadShaderText(const char* fileName, GLchar** dst)
{
    GLchar* text = NULL;
    GLint length = 0;

    FILE* fp;
    
    fopen_s(&fp, fileName, "rb");

    if (fp == NULL) { 
        printf("Unable to load \"%s\"\n", fileName);
        fclose(fp);
        return false; 
    }

    //get the length of file
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    text = (GLchar*)malloc(sizeof(char) * (length + 1)); //plus 1 for the last \0
    if (text == NULL) {
        printf("Unable to allocate memory.\n", fileName);
        fclose(fp);
        return false;
    }
    
    fread(text, 1, length, fp);
    text[length] = '\0';
    *dst = text;

    fclose(fp);
    return true;
}

bool Shader::CompileShader(const char* shader, int index, int shader_type)
{
    GLint success;

    const GLchar* shaderArray[1];
    shaderArray[0] = shader;
    shaders[index] = glCreateShader(shader_type);
    glShaderSource(shaders[index], 1, shaderArray, NULL);

    // Compile shaders and check for any errors
    glCompileShader(shaders[index]);
    glGetShaderiv(shaders[index], GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar info[MAX_INFO_SIZE];
        glGetShaderInfoLog(shaders[index], MAX_INFO_SIZE, NULL, info);
        printf("Shader %d compilation failed!\n", index);
        printf("Info: %s\n", info);
        return false;
    }

    return true;
}

bool Shader::AttachShader(int index)
{
    GLint success;

    glAttachShader(program, shaders[index]);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLchar info[MAX_INFO_SIZE];
        glGetProgramInfoLog(program, MAX_INFO_SIZE, NULL, info);
        printf("Program %d linkage failed!\n", index);
        printf("Info: %s\n", info);
        return false;
    }

    validation = GL_FALSE;
    return true;
}

bool Shader::Validate()
{
    if (!validation)
    {
        GLint success;

        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[MAX_INFO_SIZE];
            glGetProgramInfoLog(program, MAX_INFO_SIZE, NULL, infoLog);
            printf("Error in program validation!\n");
            printf("Info log: %s\n", infoLog);
            return false;
        }

        validation = GL_TRUE;
        return true;
    }

    printf("The Shader is already validated!\n");
    return true;
}

bool Shader::SetUniform(const char* uniform, int type, void* value, int count, GLboolean Transpose)
{
    GLint i = glGetUniformLocation(program, uniform);

    if (i == -1)
    {
        std::cout << "Uniform : " << uniform << " is not found!\n";
        return false;
    }

    switch (type)
    {
    case UNI_INT_1:
        glUniform1i(i, *(GLint*)value);
        break;
    case UNI_FLOAT_1:
        glUniform1f(i, *((GLfloat*)value));
        break;
    case UNI_VEC_1:
        glUniform1fv(i, count, (GLfloat*)value);
        break;
    case UNI_VEC_2:
        glUniform2fv(i, count, (GLfloat*)value);
        break;
    case UNI_VEC_3:
        glUniform3fv(i, count, (GLfloat*)value);
        break;
    case UNI_VEC_4:
        glUniform4fv(i, count, (GLfloat*)value);
        break;
    case UNI_MATRIX_2:
        glUniformMatrix2fv(i, count, Transpose, (GLfloat*)value);
        break;
    case UNI_MATRIX_3:
        glUniformMatrix3fv(i, count, Transpose, (GLfloat*)value);
        break;
    case UNI_MATRIX_4:
        glUniformMatrix4fv(i, count, Transpose, (GLfloat*)value);
        break;
    case UNI_TEXTURE:
        glUniform1i(i, count);
        break;
    }

    return true;
}

GLint Shader::GetProgram()
{
    return program;
}