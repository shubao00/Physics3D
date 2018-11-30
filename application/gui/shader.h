#pragma once

#include <iostream>
#include <string>
#include <map>

#include "../engine/math/mat3.h"
#include "../engine/math/mat4.h"

struct ShaderSource {
	std::string vertexSource;
	std::string fragmentSource;
};

ShaderSource parseShader(const std::string& path);
ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath);

class Shader {
public:
	Shader() {};
	Shader(const std::string& vertexShader, const std::string& fragmentShader);
	Shader(ShaderSource shaderSource) : Shader(shaderSource.vertexSource, shaderSource.fragmentSource) {};
	void createUniform(std::string uniform);
	void setUniform(std::string uniform, int value);
	void setUniform(std::string uniform, float value);
	void setUniform(std::string uniform, double value);
	void setUniform(std::string uniform, Vec3 value);
	void setUniform(std::string uniform, Mat4 value);
	void bind();
	void unbind();
	void close();
	unsigned int getId();
private:
	unsigned int id;
	std::map<std::string, int> uniforms;
};
