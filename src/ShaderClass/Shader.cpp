#include "ShaderClass/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderClass/FeatureFlags.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    unsigned int vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    
	checkCompileErrors(vertex, "VERTEX");
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const char* computePath)
{
    std::string computeCode;
    std::ifstream cShaderFile;

    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open files
        cShaderFile.open(computePath);
        std::stringstream cShaderStream;
        // Read file's buffer contents into streams
        cShaderStream << cShaderFile.rdbuf();
        // Close file handlers
        cShaderFile.close();
        // Convert stream into string
        computeCode = cShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* cShaderCode = computeCode.c_str();

    unsigned int compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute,1,  &cShaderCode, NULL);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(compute);
}

Shader::Shader(uint32_t shaderFlags)
{
    const char* vertexPath = "Shaders/ForwardPlus/Object.vert";
    const char* fragmentPath = "Shaders/ForwardPlus/Object.frag";

    // Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }


    // Set Shader Flags
    std::string defines = getShaderDefines(shaderFlags);
    std::string placeholder = "// <DEFINES_PLACEHOLDER>";
    size_t pos = fragmentCode.find(placeholder);

    std::string finalFragmentCode = fragmentCode;
    if (pos != std::string::npos)
    {
        // If the placeholder is found, replace it with our defines
        finalFragmentCode.replace(pos, placeholder.length(), defines);
    }
    else
    {
        // Fallback or error if placeholder isn't in the shader file
        std::cerr << "Warning: <DEFINES_PLACEHOLDER> not found in fragment shader." << std::endl;
        // You could just prepend here as a fallback if you want
        // finalFragmentCode = defines + fragmentCode;
    }

    // Convert to C Style String
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = finalFragmentCode.c_str();

    // Compile shaders
    unsigned int vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    checkCompileErrors(vertex, "VERTEX");
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::use()
{
    glUseProgram(ID);
}

//-------------------------------------------------------------------------
// Scalar Uniform Setters
//-------------------------------------------------------------------------
void Shader::setUniformValue(const std::string& name, float x, float y, float z) const
{
    glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setUniformValue(const std::string& name, int x, int y, int z) const
{
    glUniform3i(getUniformLocation(name), x, y, z);
}

void Shader::setUniformValue(const std::string& name, double x, double y, double z) const
{
    glUniform3d(getUniformLocation(name), x, y, z);
}

void Shader::setUniformValue(const std::string& name, bool value) const
{
    glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void Shader::setUniformValue(const std::string& name, int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniformValue(const std::string& name, unsigned int value) const
{
    glUniform1ui(getUniformLocation(name), value);
}

void Shader::setUniformValue(const std::string& name, float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniformValue(const std::string& name, double value) const
{
    glUniform1d(getUniformLocation(name), value);
}


//-------------------------------------------------------------------------
// Vector Uniform Setters
//-------------------------------------------------------------------------

// 2-component vectors
void Shader::setUniformValue(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::ivec2& value) const
{
    glUniform2iv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::uvec2& value) const
{
    glUniform2uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::dvec2& value) const
{
    glUniform2dv(getUniformLocation(name), 1, glm::value_ptr(value));
}

// 3-component vectors
void Shader::setUniformValue(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::ivec3& value) const
{
    glUniform3iv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::uvec3& value) const
{
    glUniform3uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::dvec3& value) const
{
    glUniform3dv(getUniformLocation(name), 1, glm::value_ptr(value));
}

// 4-component vectors
void Shader::setUniformValue(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::ivec4& value) const
{
    glUniform4iv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::uvec4& value) const
{
    glUniform4uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::dvec4& value) const
{
    glUniform4dv(getUniformLocation(name), 1, glm::value_ptr(value));
}

//-------------------------------------------------------------------------
// Matrix Uniform Setters
//-------------------------------------------------------------------------

// 2x2 matrices
void Shader::setUniformValue(const std::string& name, const glm::mat2& value) const
{
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::dmat2& value) const
{
    glUniformMatrix2dv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

// 3x3 matrices
void Shader::setUniformValue(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::dmat3& value) const
{
    glUniformMatrix3dv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

// 4x4 matrices
void Shader::setUniformValue(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::dmat4& value) const
{
    glUniformMatrix4dv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

// Non-square matrices
void Shader::setUniformValue(const std::string& name, const glm::mat2x3& value) const
{
    glUniformMatrix2x3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::mat3x2& value) const
{
    glUniformMatrix3x2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::mat2x4& value) const
{
    glUniformMatrix2x4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::mat4x2& value) const
{
    glUniformMatrix4x2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::mat3x4& value) const
{
    glUniformMatrix3x4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformValue(const std::string& name, const glm::mat4x3& value) const
{
    glUniformMatrix4x3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

int Shader::getUniformLocation(const std::string& name) const
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        // It's often useful to know if a uniform wasn't found.
        // This can happen if the uniform is not used in the shader,
        // as the compiler may optimize it out.
        std::cerr << "Warning: uniform '" << name << "' not found." << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}

std::string Shader::getShaderDefines(uint32_t shaderFlags)
{
    std::string defines;
    if ((shaderFlags & SHADER_FEATURE_Diffuse_MAP) != 0)
    {
        defines += "#define USE_DiffuseMap\n";
    }
    if ((shaderFlags & SHADER_FEATURE_NORMAL_MAP) != 0)
    {
        defines += "#define USE_NormalMap\n";
    }
    if ((shaderFlags & SHADER_FEATURE_SPECULAR_MAP) != 0)
    {
        defines += "#define USE_SpecularMap\n";
    }
    if ((shaderFlags & SHADER_FEATURE_ALPHA_TEST) != 0)
    {
        defines += "#define USE_AlphaTest\n";
    }

    return defines;
}