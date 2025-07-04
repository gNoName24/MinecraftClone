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

#include "MCCPP/imgui/init.h"

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

	// Включение ImGui
	ImGuiLayer::Init(window);

	// Если окно ресайзнулось
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Нажалась клавиша
	glfwSetKeyCallback(window, key_callback);
	// Движение мыши
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwGetCursorPos(window, &camera.lastX, &camera.lastY);

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

	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);

	glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 materialAmbient = glm::vec3(0.24725f, 0.1995f, 0.0745f);
	glm::vec3 materialDiffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f);
	glm::vec3 materialSpecular = glm::vec3(0.628281f, 0.555802f, 0.366065f);
	float materialShininess = 0.4 * 128.0;

	while(!glfwWindowShouldClose(window)) {
		ImGuiLayer::UpdateMousePosition(window);

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

		ImGuiLayer::BeginFrame();

		camera.do_movement(deltaTime);
		//camera.updateCamera();

		camera.setMatsToShader(&shader);
		camera.setMatsToShader(&shaderLightCube);

		glm::mat4 model = glm::mat4(1.0f);
		shader.setMat4fv("model", model);
		shaderLightCube.setMat4fv("model", model);

		glm::vec3 lightPos(8.0f, 7.5f, 8.0f);
		shader.setVec3f("light.position", lightPos);

		shader.setVec3f("viewPos", camera.position);

		ImGui::Begin("Window");
		ImGui::DragFloat3("lightColor", &lightColor.x, 0.01f);
		ImGui::DragFloat3("objectColor", &objectColor.x, 0.01f);
		ImGui::DragFloat3("light.ambient", &lightAmbient.x, 0.01f);
		ImGui::DragFloat3("light.diffuse", &lightDiffuse.x, 0.01f);
		ImGui::DragFloat3("light.specular", &lightSpecular.x, 0.01f);
		ImGui::DragFloat3("material.ambient", &materialAmbient.x, 0.01f);
		ImGui::DragFloat3("material.diffuse", &materialDiffuse.x, 0.01f);
		ImGui::DragFloat3("material.specular", &materialSpecular.x, 0.01f);
		ImGui::DragFloat("material.shininess", &materialShininess, 0.2f);
		ImGui::End();

		shader.setVec3f("lightColor", lightColor);
		shader.setVec3f("objectColor", objectColor);

		shader.setVec3f("light.ambient", lightAmbient);
		shader.setVec3f("light.diffuse", lightDiffuse);
		shader.setVec3f("light.specular", lightSpecular);

		shader.setVec3f("material.ambient", materialAmbient);
		shader.setVec3f("material.diffuse", materialDiffuse);
		shader.setVec3f("material.specular", materialSpecular);
		shader.setFloat("material.shininess", materialShininess);

		glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(shaderLightCube.shaderProgram);
		glDrawArrays(GL_TRIANGLES, 36, 36);

		//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		ImGuiLayer::EndFrame();
		glfwSwapBuffers(window);
	}

	std::cout << "Hello World!";
	ImGuiLayer::Shutdown();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			double x, y;
			glfwGetCursorPos(window, &x, &y);
			ImGuiIO& io = ImGui::GetIO();
			io.MousePos = ImVec2((float)x, (float)y);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	camera.key_callback(window, key, scancode, action, mode);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;

	camera.mouse_callback(window, xpos, ypos);
}