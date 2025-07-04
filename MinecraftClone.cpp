#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Для картинок
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <windows.h>
#include <locale>

#include <filesystem>

#include <vector>
#include <iostream>

#include <fstream>
#include <sstream>
#include <string>

#include <FastNoiseLite/FastNoiseLite.h>

#include "MCCPP/Shader.h"
#include "MCCPP/Camera.h"

#include "MCCPP/ObjectTool.h"

using namespace std;

float WIDTH = 800.0f;
float HEIGHT = 600.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void updateDeltaTime() {
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

Camera camera;

std::string readFile(const std::string& filename) {
	std::ifstream file(filename);
	if(!file) return {};

	std::ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main() {
	// Русский язык в консоли
	std::locale::global(std::locale("Russian"));
	std::wcout.imbue(std::locale());
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);

	std::cout << "Рабочая директория: " << filesystem::current_path() << std::endl;

	// Инициализация GLFW с OpenGL 3.3 (Core Profile)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Создание окна
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft Clone", NULL, NULL);
	if(window == NULL) {
		std::cout << "Окно не создалось, хз почему" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Инициализация GLAD
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Почему-то GLAD не инициализировался, разбирайся" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	// Включи если хочется снять ограничение фпс
	// glfwSwapInterval(0);

	// Если окно ресайзнулось
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Нажалась клавиша
	glfwSetKeyCallback(window, key_callback);
	// Движение мыши
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwGetCursorPos(window, &camera.lastX, &camera.lastY);

	// Генерация плоскости из кучи полигонов, уже не актуально
	/*std::vector<float> map = GenerateMesh(128, 128);
	ApplyPerlinNoise(map, 128, 128);*/

	Shader shader(
		(std::filesystem::current_path() / "data/shaders/shader.vert").string(),
		(std::filesystem::current_path() / "data/shaders/shader.frag").string()
	);
	shader.compile();

	Shader shaderLightCube(
		(std::filesystem::current_path() / "data/shaders/lightCube.vert").string(),
		(std::filesystem::current_path() / "data/shaders/lightCube.frag").string()
	);
	shaderLightCube.compile();

	vector<GLfloat> map = concat<GLfloat>({
		// Освещаемый куб
		ObjectTool::polyCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(16.0f, 4.0f, 16.0f)),
		// Источник цвета
		ObjectTool::polyCube(glm::vec3(8.0f, 7.5f, 8.0f), glm::vec3(4.0f, 4.0f, 4.0f)),
	});

	vector<GLuint> indices = {};

	/*FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(0.05f);

	int width = 256;
	int height = 256;
	for(int z = 0; z < height; z++) {
		for(int x = 0; x < width; x++) {
			GLfloat glx = static_cast<GLfloat>(x);
			GLfloat glz = static_cast<GLfloat>(z);

			map.insert(map.end(), {
				glx,     noise.GetNoise((float)x, (float)z) * 10.0f, glz,
				glx + 1, noise.GetNoise((float)x+1, (float)z) * 10.0f, glz,
				glx,     noise.GetNoise((float)x, (float)z+1) * 10.0f, glz + 1,
				glx + 1, noise.GetNoise((float)x+1, (float)z+1) * 10.0f, glz + 1
			});

			GLuint start = (x + z * width) * 4;
			indices.insert(indices.end(), {
				start + 0, start + 1, start + 2,
				start + 2, start + 1, start + 3
			});
		}
	}*/

	// Устарело
	/*for (int i = 0; i < width * height; i++) {
		vector<GLfloat> preset = {
			1.0f * i, 0.0f, 1.0f * i,
			(1.0f * i) + 1, 0.0f, 0.0f,
			0.0f, 0.0f, (1.0f * i) + 1,
			(1.0f * i) + 1, 0.0f, (1.0f * i) + 1,
		};
		map.insert(map.end(), preset.begin(), preset.end());

		vector<GLuint> preset2 = {
			0 + ((GLuint)i*4), 1 + ((GLuint)i * 4), 2 + ((GLuint)i * 4),
			2 + ((GLuint)i * 4), 1 + ((GLuint)i * 4), 3 + ((GLuint)i * 4)
		};
		indices.insert(indices.end(), preset2.begin(), preset2.end());
	}*/

	/*auto [map, indices] = ObjectTool::polyRectEBO({
		0.0f, 1.0f, 0.0f,
		1.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
	});*/

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, map.size() * sizeof(GLfloat), map.data(), GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);*/

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Позиция
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Позиция нормали
	glEnableVertexAttribArray(1);
	 
	glBindVertexArray(0);

	std::cout << "lightCube size: " << map.size() << std::endl;

	double lastTime = 0.0;
	int nbFrames = 0;

	glEnable(GL_DEPTH_TEST);

	while(!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		nbFrames++;

		// Печать каждые 1.0 секунды
		if (currentTime - lastTime >= 1.0) {
			std::cout << "FPS: " << nbFrames << std::endl;
			nbFrames = 0;
			lastTime = currentTime;
		}

		updateDeltaTime();
		glfwPollEvents();

		camera.do_movement(deltaTime);
		camera.updateCamera();

		camera.setMatsToShader(&shader);
		camera.setMatsToShader(&shaderLightCube);

		glm::mat4 model = glm::mat4(1.0f);
		shader.setMat4fv("model", model);
		model = glm::translate(model, glm::vec3(sin(glfwGetTime()) * 12.0f, 0.0f, cos(glfwGetTime()) * 12.0f));
		shaderLightCube.setMat4fv("model", model);

		shader.setVec3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setVec3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

		glm::vec3 lightPos(8.0f + (sin(glfwGetTime()) * 12.0f), 7.5f, 8.0f + (cos(glfwGetTime()) * 12.0f));
		shader.setVec3f("lightPos", lightPos);

		shader.setVec3f("viewPos", camera.position);

		glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(shaderLightCube.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 36, 36);

		//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	std::cout << "Hello World!";
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	camera.key_callback(window, key, scancode, action, mode);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.mouse_callback(window, xpos, ypos);
}