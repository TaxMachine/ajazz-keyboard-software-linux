//
// Created by taxmachine on 22/02/25.
//

#include "app.hpp"

#include <cstdlib>
#include <iostream>
#include <utility>
#include <imgui.h>
#include "../backends/imgui_impl_opengl3.h"
#include "../backends/imgui_impl_glfw.h"

static void glfw_error_callback(const int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

AjazzGUI::AjazzGUI(std::string applicationName) : m_applicationName(std::move(applicationName)) {
    initWindow();
    initIMGUI();
}

AjazzGUI::~AjazzGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void AjazzGUI::run() {
    mainLoop();
}

void AjazzGUI::initWindow() {
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    this->m_window = glfwCreateWindow(800, 720, this->m_applicationName.c_str(), nullptr, nullptr);
    if (!this->m_window) {
        delete this;
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
}

void AjazzGUI::initIMGUI() const {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsResizeFromEdges = false;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void AjazzGUI::mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        this->render();
        ImGui::Render();

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
        int app_width, app_height;
        glfwGetFramebufferSize(m_window, &app_width, &app_height);
        glViewport(0, 0, app_width, app_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void AjazzGUI::render() {
    ImGui::Begin(this->m_applicationName.c_str(), nullptr, ImGuiWindowFlags_NoResize |
                                                            ImGuiWindowFlags_NoCollapse |
                                                            ImGuiWindowFlags_NoMove |
                                                            ImGuiWindowFlags_NoSavedSettings |
                                                            ImGuiWindowFlags_NoBringToFrontOnFocus |
                                                            ImGuiWindowFlags_NoDocking |
                                                            ImGuiWindowFlags_NoNavFocus |
                                                            ImGuiWindowFlags_NoSavedSettings);
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    ImGui::SetWindowSize(ImVec2(static_cast<float>(width), (static_cast<float>(height) - this->m_titlebarHeight)), ImGuiCond_Always);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowContentSize(ImVec2(static_cast<float>(width), static_cast<float>(height)));
    ImGui::SetWindowFontScale(1.0f);
    ImGui::SetNextWindowSizeConstraints(ImVec2(static_cast<float>(width), static_cast<float>(height)), ImVec2(static_cast<float>(width), static_cast<float>(height)));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 15);

    if (ImGui::BeginTabBar("Settings", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("LED Modes")) {
            this->ModeWindow();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Macros")) {
            this->MacrosWindow();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("TFT Screen")) {
            this->TFTScreenWindow();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Theme Editor")) {
            this->ThemeEditorWindow();
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();

    ImGui::PopStyleVar(2);
    ImGui::End();
}
