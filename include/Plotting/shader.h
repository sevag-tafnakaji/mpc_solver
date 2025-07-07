#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>

class Shader
{
public:
    // state
    unsigned int ID;
    // constructor
    Shader() {}
    // sets the current shader as active
    Shader &Use();
    // compiles the shader from given source code
    void Compile(const char *vertexSource, const char *fragmentSource,
                 const char *geometrySource = nullptr); // note: geometry source code is optional

private:
    // checks if compilation or linking failed and if so, print the error logs
    void checkCompileErrors(unsigned int object, std::string type);
};

#endif
