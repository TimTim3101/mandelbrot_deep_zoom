#ifndef SHADER_H
#define SHADER_H

#include "../third-party/glad/glad.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
  private:
    GLuint ProgramID;
    GLuint compileShader(std::string source, GLenum type);
    std::string ReadShader(const char *FilePath);

  public:
    Shader(const char *vShaderPath, const char *fShaderPath);
    ~Shader();

    void use() const;
    GLuint getID() const;
};

#endif
