#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <Eigen/Eigen>

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

    void SetVector3f(const char *name, float x, float y, float z, bool useShader);
    void SetVector3f(const char *name, const Eigen::Vector3f &value, bool useShader);

private:
    // checks if compilation or linking failed and if so, print the error logs
    void checkCompileErrors(unsigned int object, std::string type);
};

#endif
