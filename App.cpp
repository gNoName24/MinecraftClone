// === Новый функционал: установка и ломание блоков ===
// Добавлено определение блока под прицелом, установка нового блока на правую кнопку, удаление — на левую

#include "App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <windows.h>
#include <locale>
#include <filesystem>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <ctime>
#include <FastNoiseLite/FastNoiseLite.h>

#include "MCCPP/Shader.h"
#include "MCCPP/Camera.h"
#include "MCCPP/ObjectTool.h"
#include "World.h"

extern Camera camera;
Camera camera;

float WIDTH = 1280.0f;
float HEIGHT = 720.0f;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool mousePressedL = false;
bool mousePressedR = false;

namespace {
    void updateDeltaTime() {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        camera.key_callback(window, key, scancode, action, mode);
    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        camera.mouse_callback(window, xpos, ypos);
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            mousePressedL = (action == GLFW_PRESS);
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mousePressedR = (action == GLFW_PRESS);
    }

    // Raycasting
    bool raycastBlock(World& world, glm::vec3 origin, glm::vec3 dir, float maxDist, glm::ivec3& hitBlock, glm::ivec3& normal) {
        glm::vec3 pos = origin;
        glm::ivec3 lastBlock = glm::floor(pos);

        for (int i = 0; i < int(maxDist * 100); ++i) {
            pos += dir * 0.01f;
            glm::ivec3 blockPos = glm::floor(pos);
            if (blockPos != lastBlock) {
                if (world.getBlock(blockPos.x, blockPos.y, blockPos.z) != 0) {
                    hitBlock = blockPos;
                    normal = lastBlock - blockPos; // ← правильная нормаль — направление *вне блока*
                    return true;
                }
                lastBlock = blockPos;
            }
        }

        return false;
    }


}

void App::run() {
    std::locale::global(std::locale("Russian"));
    std::wcout.imbue(std::locale());
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft Clone", NULL, NULL);
    if (!window) { std::cerr << "Не удалось создать окно\n"; glfwTerminate(); return; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Не удалось инициализировать GLAD\n";
        return;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &camera.lastX, &camera.lastY);

    glEnable(GL_DEPTH_TEST);

    Shader shader("data/shaders/shader.vert", "data/shaders/shader.frag");
    shader.compile();
    Shader shadowShader("data/shaders/shadow_depth.vert", "data/shaders/shadow_depth.frag");
    shadowShader.compile();

    camera.cameraShader = &shader;

    World world(256, 64, 256); // теперь можно без размеров
    world.generate();

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));
    glm::mat4 lightProjection = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, 1.0f, 1000.0f);
    glm::mat4 lightView = glm::lookAt(-lightDir * 200.0f, glm::vec3(0.0f), glm::vec3(0, 1, 0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    shader.use();
    shader.setInt("shadowMap", 0);

    double lastTime = 0.0;
    int nbFrames = 0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            std::cout << "FPS: " << nbFrames << std::endl;
            nbFrames = 0;
            lastTime = currentTime;
        }

        updateDeltaTime();
        glfwPollEvents();
        camera.do_movement(deltaTime);
        camera.updateCamera();

        // === Управление блоками ===
        glm::ivec3 hitBlock, normal;
        if (raycastBlock(world, camera.position, camera.front, 15.0f, hitBlock, normal)) {
            if (mousePressedL) {
                world.setBlock(hitBlock.x, hitBlock.y, hitBlock.z, 0);
                mousePressedL = false;
            }
            if (mousePressedR) {
                glm::ivec3 place = hitBlock + normal;
                if (world.getBlock(place.x, place.y, place.z) == 0) {
                    world.setBlock(place.x, place.y, place.z, 3);
                }
                mousePressedR = false;
            }
        }


        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        shadowShader.use();
        shadowShader.setMat4fv("lightSpaceMatrix", lightSpaceMatrix);
        shadowShader.setMat4fv("model", glm::mat4(1.0f));
        glCullFace(GL_FRONT);
        world.update();
        world.render();
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("globalLightDir", lightDir);
        shader.setVec3("globalLightColor", glm::vec3(0.8f));
        shader.setVec3("ambientLightColor", glm::vec3(0.2f));
        shader.setVec3("viewPos", camera.position);
        shader.setMat4fv("model", glm::mat4(1.0f));
        shader.setMat4fv("view", camera.getViewMatrix());
        shader.setMat4fv("projection", camera.getProjection());
        shader.setMat4fv("lightSpaceMatrix", lightSpaceMatrix);

        shader.setInt("numPointLights", 0);
        shader.setVec4("pointLights[0].position", glm::vec4(camera.position.x, camera.position.y, camera.position.z, 100));
        shader.setVec3("pointLights[0].color", glm::vec3(2.0f, 0.5f, 0.5f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        world.render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
