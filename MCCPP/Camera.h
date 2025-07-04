#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"

using namespace std;

class Camera {
public:
	glm::vec3 position = glm::vec3(0.0f);

	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	GLdouble lastX = 0, lastY = 0;
	GLdouble mouseX = 0, mouseY = 0;
	GLfloat yaw = -90.0f; // Влево или вправо
	GLfloat pitch = 0.0f; // Наклон вперед или вниз

	// Shader* cameraShader; // Указатель на класс шейдера. Указатель потому-что со временем может появится необходимость менять шейдеры
	//						 Теперь не нужен

	bool keys[1024];

	bool mouseLock = true;

	// Основные настройки камеры
	float fov = 70.0f;
	float renderMinDistance = 0.1f;
	float renderMaxDistance = 1024.0f;

	float cameraSpeed = 75.0f;

	glm::vec2 screenSize = glm::vec2(800.0f, 600.0f); // Сюда надо задавать размер окна

	/*void updateCamera() {
		// Создание матриц
		// glm::mat4 model = glm::mat4(1.0f); // Матрица модели, не используется в камере
		view = glm::mat4(1.0f); // Матрица вида
		projection = glm::mat4(1.0f); // Матрица проекции

		view = glm::lookAt(position, position + front, up);

		projection = glm::perspective(glm::radians(fov), screenSize.x / screenSize.y, renderMinDistance, renderMaxDistance);

		// Передача матриц в шейдер
		// Тепеь не нужно, используется другая функция для того чтобы можно было передавать матрицы сразу в несколько шейдеров
		/*cameraShader->setMat4fv("view", view);
		cameraShader->setMat4fv("projection", projection);*/
	//}

	void setMatsToShader(Shader* shader) {
		if(shader == nullptr) {
			cout << "shader не установлен" << endl;
			return;
		}
		view = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);
		view = glm::lookAt(position, position + front, up);
		shader->setMat4fv("view", view);
		projection = glm::perspective(glm::radians(fov), screenSize.x / screenSize.y, renderMinDistance, renderMaxDistance);
		shader->setMat4fv("projection", projection);
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if(action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;

		if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
			if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
				mouseLock = true;
			} else {
				mouseLock = false;
			}
		}
	}
	void do_movement(const float &deltaTime) {
		GLfloat speed = cameraSpeed * deltaTime;
		if(keys[GLFW_KEY_W])
			position += speed * front;
		if(keys[GLFW_KEY_S])
			position -= speed * front;
		if(keys[GLFW_KEY_A])
			position -= glm::normalize(glm::cross(front, up)) * speed;
		if(keys[GLFW_KEY_D])
			position += glm::normalize(glm::cross(front, up)) * speed;

		// Вверх и вниз
		if(keys[GLFW_KEY_E])
			position += speed * glm::normalize(up);
		if(keys[GLFW_KEY_Q])
			position += speed * glm::normalize(-up);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		if (!mouseLock)
			return;

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		GLfloat sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if(pitch > 89.0f)
			pitch = 89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		this->front = glm::normalize(front);
	}
};