#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>

class Shader
{
public:
    // The program ID
    unsigned int ID;

    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // Destructor to clean up the shader program
    ~Shader();

    int Shader::getUniformLocation(const std::string& name) const;

    // Use/activate the shader
    void use();

    

    //-------------------------------------------------------------------------
    // Scalar Uniform Setters
    //-------------------------------------------------------------------------
    void setUniformValue(const std::string& name, float x, float y, float z) const;
    void setUniformValue(const std::string& name, int x, int y, int z) const;
    void setUniformValue(const std::string& name, double x, double y, double z) const;
    void setUniformValue(const std::string& name, bool value) const;
    void setUniformValue(const std::string& name, int value) const;
    void setUniformValue(const std::string& name, unsigned int value) const;
    void setUniformValue(const std::string& name, float value) const;
    void setUniformValue(const std::string& name, double value) const;

    //-------------------------------------------------------------------------
    // Vector Uniform Setters
    //-------------------------------------------------------------------------
    void setUniformValue(const std::string& name, const glm::vec2& value) const;
    void setUniformValue(const std::string& name, const glm::ivec2& value) const;
    void setUniformValue(const std::string& name, const glm::uvec2& value) const;
    void setUniformValue(const std::string& name, const glm::dvec2& value) const;

    void setUniformValue(const std::string& name, const glm::vec3& value) const;
    void setUniformValue(const std::string& name, const glm::ivec3& value) const;
    void setUniformValue(const std::string& name, const glm::uvec3& value) const;
    void setUniformValue(const std::string& name, const glm::dvec3& value) const;

    void setUniformValue(const std::string& name, const glm::vec4& value) const;
    void setUniformValue(const std::string& name, const glm::ivec4& value) const;
    void setUniformValue(const std::string& name, const glm::uvec4& value) const;
    void setUniformValue(const std::string& name, const glm::dvec4& value) const;

    //-------------------------------------------------------------------------
    // Matrix Uniform Setters
    //-------------------------------------------------------------------------
    void setUniformValue(const std::string& name, const glm::mat2& value) const;
    void setUniformValue(const std::string& name, const glm::dmat2& value) const;
    void setUniformValue(const std::string& name, const glm::mat3& value) const;
    void setUniformValue(const std::string& name, const glm::dmat3& value) const;
    void setUniformValue(const std::string& name, const glm::mat4& value) const;
    void setUniformValue(const std::string& name, const glm::dmat4& value) const;

    void setUniformValue(const std::string& name, const glm::mat2x3& value) const;
    void setUniformValue(const std::string& name, const glm::mat3x2& value) const;
    void setUniformValue(const std::string& name, const glm::mat2x4& value) const;
    void setUniformValue(const std::string& name, const glm::mat4x2& value) const;
    void setUniformValue(const std::string& name, const glm::mat3x4& value) const;
    void setUniformValue(const std::string& name, const glm::mat4x3& value) const;


private:
    // Private helper function to check for compile/link errors
    void checkCompileErrors(unsigned int shader, std::string type);
    mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;
};