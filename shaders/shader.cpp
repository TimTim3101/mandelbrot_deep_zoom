#include "shader.h"

std::string Shader::ReadShader(const char *FilePath) {
    std::ifstream f(FilePath);
    if (!f.is_open()) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
        return "";
    }
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}

GLuint Shader::compileShader(std::string source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char *code = source.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED: " << infoLog;
    }

    return shader;
}

Shader::Shader(const char *vShaderPath, const char *fShaderPath) {
    std::string vCode = ReadShader(vShaderPath);
    std::string fCode = ReadShader(fShaderPath);

    GLuint vertexShader = compileShader(vCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fCode, GL_FRAGMENT_SHADER);

    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vertexShader);
    glAttachShader(ProgramID, fragmentShader);
    glLinkProgram(ProgramID);

    int success;
    char infoLog[512];
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINKING_FAILED: " << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(ProgramID);
}

void Shader::use() const {
    glUseProgram(ProgramID);
}

GLuint Shader::getID() const {
    return ProgramID;
}
