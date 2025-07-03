#pragma once

#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Shader {
public:
	Shader(const string& vsp, const string& fsp) : vertexShaderPath(vsp), fragmentShaderPath(fsp) {}

	string vertexShaderPath;
	string fragmentShaderPath;

	unsigned int shaderProgram;

	void compile() {
		const char* bufferChar;
		ifstream vertexFile(vertexShaderPath);
		ifstream fragmentFile(fragmentShaderPath);

		if(!vertexFile.is_open() || !fragmentFile.is_open()) {
			throw runtime_error("Почему-то файлы не открылись");
		}

		// Создание вершинного шейдера
		stringstream vBuffer;
		vBuffer << vertexFile.rdbuf();
		std::string vertexCode = vBuffer.str();
		const char* vertexChar = vertexCode.c_str();

		unsigned int vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexChar, NULL);
		glCompileShader(vertexShader);
		getCompileError(vertexShader, "vertex");

		// Создание фрагментного шейдера
		stringstream fBuffer;
		fBuffer << fragmentFile.rdbuf();
		std::string fragmentCode = fBuffer.str();
		const char* fragmentChar = fragmentCode.c_str();

		unsigned int fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentChar, NULL);
		glCompileShader(fragmentShader);
		getCompileError(fragmentShader, "fragment");

		// Создание программы
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		cout << "Шейдер скомпилировался" << endl;
	}

	void getCompileError(unsigned int shader, const string typeShader) {
		int  success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "О боже, ошибка " + typeShader + " шейдера:\n      " << infoLog;
		} else {
			std::cout << typeShader + " успешно скомпилировался, поздравляю\n";
		}
	}

	// Вспомогательные функции
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const {
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}
	void setMat4fv(const std::string& name, glm::mat4 value) const {
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
};