#pragma once

#include <GLFW/glfw3.h>

#include <imgui.h>

// Бэкенды ImGui
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace ImGuiLayer {
    inline void Init(GLFWwindow* window, const char* glsl_version = "#version 330") {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        // Инициализация под OpenGL (GLFW)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    inline void UpdateMousePosition(GLFWwindow* window) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
    }

    inline void BeginFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    inline void EndFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    inline void Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

}