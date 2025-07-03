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

float WIDTH = 1280.0f;
float HEIGHT = 720.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void updateDeltaTime() {
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

Camera camera;

/*std::vector<float> GenerateMesh(int width, int height) {
    // Тут целый квадрат без EBO, нужно потом это реализовать
    std::vector<float> preset = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    int presetVertexCount = preset.size() / 3;
    std::vector<float> map(width * height * presetVertexCount * 3);

    for(int i = 0; i < width * height; ++i) {
        int chunkX = i % width;
        int chunkZ = i / width;

        for(int j = 0; j < presetVertexCount; ++j) {
            int mapIndex = (i * presetVertexCount + j) * 3;

            float x = preset[j * 3 + 0] + chunkX;
            float y = 0.0f;
            float z = preset[j * 3 + 2] + chunkZ;

            map[mapIndex + 0] = x;
            map[mapIndex + 1] = y;
            map[mapIndex + 2] = z;
        }
    }

    return map;
}

void ApplyPerlinNoise(std::vector<float>& map, int width, int height) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.05f);

    for(int i = 0; i < map.size(); i += 3) {
        float x = map[i];
        float z = map[i + 2];
        map[i + 1] = noise.GetNoise(x, z) * 16.0f;
    }
}*/

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if(!file) return {};

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Полезная штука
template<typename T>
std::vector<T> concat(std::initializer_list<std::vector<T>> lists) {
    std::vector<T> result;
    for (const auto& l : lists)
        result.insert(result.end(), l.begin(), l.end());
    return result;
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

    glfwSwapInterval(0);

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

	camera.cameraShader = &shader;

    vector<GLfloat> map = concat<GLfloat>({
        //ObjectTool::polygon(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
        //ObjectTool::polygon(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
        //ObjectTool::polygon(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
        //ObjectTool::polygon(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
        //ObjectTool::polyRect(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 10.0f))
	});

    vector<GLuint> indices = {};

    FastNoiseLite noise;
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
	}
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
     
    glBindVertexArray(0);

    double lastTime = 0.0;
    int nbFrames = 0;

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

        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4fv("model", model);

        glUseProgram(shader.shaderProgram);

        glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, map.size() / 3);
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
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