//
// Created by taxmachine on 23/02/25.
//

#include <future>

#include "../app.hpp"

#include <imgui.h>
#include <iostream>

#include "../backends/imgui_impl_opengl3.h"
#include "../backends/imgui_impl_glfw.h"
#include "../../constants.hpp"
#include "../../utils/rgb.hpp"

static int modeIdx = 0;
static int sleepIdx = 0;
static int directionIdx = 0;
static int selectedMode = 1;
static int selectedTime = 0;
static int selectedDirection = 0;
static bool showColorPicker = false;
static bool showColorPickerButton = false;

void AjazzGUI::changeMode() {
    uint8_t r, g, b;
    const auto keyboard_color = RGB(GetConfig()->keyboard_rgb);
    keyboard_color.toRGB(&r, &g, &b);

    if (GetConfig()->mode >= LIGHT_MODES.size() ||
        GetConfig()->direction >= DIRECTIONS.size() ||
        GetConfig()->sleep_delay >= SLEEP_DELAY.size()) {
            std::cerr << "Invalid array index!" << std::endl;
            return;
        }

    std::cout << "Changing mode to " << std::endl;
    std::cout << "Mode: " << LIGHT_MODES[GetConfig()->mode] << std::endl;
    std::cout << "RGB: " << std::to_string(r) << " " << std::to_string(g) << " " << std::to_string(b) << std::endl;
    std::cout << "Rainbow?: " << GetConfig()->rainbow << std::endl;
    std::cout << "Brightness: " << GetConfig()->brightness << std::endl;
    std::cout << "Speed: " << GetConfig()->speed << std::endl;
    std::cout << "Direction: " << DIRECTIONS.at(GetConfig()->direction) << std::endl;
    std::cout << "LED Sleep delay: " << SLEEP_DELAY.at(GetConfig()->sleep_delay) << std::endl;

    std::cout.flush();

    try {
        GetKeyboard()->openHandle();

        GetKeyboard()->setModeAsync(
                    GetConfig()->mode,
                    r,
                    g,
                    b,
                    GetConfig()->rainbow,
                    GetConfig()->brightness,
                    GetConfig()->speed,
                    GetConfig()->direction);
        GetKeyboard()->setSleepTimeAsync(GetConfig()->sleep_delay);

        GetKeyboard()->closeHandle();
    } catch (const std::exception& e) {
        std::cerr << "Error launching async task: " << e.what() << std::endl;
    }
}

void AjazzGUI::ModeWindow() {
    ImGui::BeginGroup();

    ImGui::BeginGroup();

    ImGui::Text("LED lighting modes");

    for (const auto&[mode, name] : LIGHT_MODES) {
        modeIdx++;
        if (ImGui::RadioButton(name, &selectedMode, mode)) {
            GetConfig()->mode = mode;
        }
        if (modeIdx != 4) {
            ImGui::SameLine(0, 10);
        } else {
            modeIdx = 0;
        }
    }

    ImGui::EndGroup();

    ImGui::Dummy(ImVec2(0, 10));

    ImGui::BeginGroup();
    if (ImGui::Checkbox("Rainbow", &GetConfig()->rainbow)) {
        showColorPickerButton = !GetConfig()->rainbow;
    }

    if (showColorPickerButton) {
        if (ImGui::Button("Pick Color...")) {
            showColorPicker = true;
        }
    }

    ImGui::EndGroup();

    ImGui::Dummy(ImVec2(0, 10));

    ImGui::BeginGroup();
    ImGui::SliderInt("Brightness", &GetConfig()->brightness, 0, MAX_BRIGHTNESS);
    ImGui::SliderInt("Speed", &GetConfig()->speed, 0, MAX_SPEED);

    ImGui::BeginGroup();
    for (const auto&[mode, mode_name] : SLEEP_DELAY) {
        sleepIdx++;
        if (ImGui::RadioButton(mode_name, &selectedTime, mode)) {
            GetConfig()->sleep_delay = mode;
        }
        if (sleepIdx < SLEEP_DELAY.size()) {
            ImGui::SameLine(0, 10);
        } else {
            sleepIdx = 0;
        }
    }
    ImGui::EndGroup();

    ImGui::Dummy(ImVec2(0, 10));

    ImGui::BeginGroup();
    for (const auto& [mode, directions] : DIRECTIONS_PER_MODE) {
        if (GetConfig()->mode != mode) continue;
        for (const auto& direction : directions) {
            directionIdx++;
            if (ImGui::RadioButton(DIRECTIONS.at(direction), &selectedDirection, direction)) {
                GetConfig()->direction = direction;
            }
            if (sleepIdx < DIRECTIONS.size()) {
                ImGui::SameLine(0, 10);
            } else {
                sleepIdx = 0;
            }
        }
    }
    ImGui::EndGroup();

    ImGui::Dummy(ImVec2(0, 10));

    ImGui::EndGroup();

    if (ImGui::Button("Apply")) {
        this->changeMode();
    }

    if (showColorPicker) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        GLFWwindow* detached_window = glfwCreateWindow(800, 600, "Color Picker", nullptr, this->m_window);
        if (!detached_window) {
            return;
        }

        GLFWwindow* previous_context = glfwGetCurrentContext();
        ImGuiContext* previous_imgui_context = ImGui::GetCurrentContext();

        glfwMakeContextCurrent(detached_window);

        ImGuiContext* new_context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::SetCurrentContext(new_context);

        ImGui_ImplGlfw_InitForOpenGL(detached_window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        glfwSetWindowCloseCallback(detached_window, [](GLFWwindow* window) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        });

        while (!glfwWindowShouldClose(detached_window)) {
            glfwPollEvents();

            if (!glfwGetWindowAttrib(detached_window, GLFW_VISIBLE)) {
                break;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("LED Color Picker", &showColorPicker, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                const ImVec2 delta = ImGui::GetIO().MouseDelta;
                int x, y;
                glfwGetWindowPos(detached_window, &x, &y);
                glfwSetWindowPos(detached_window, x + delta.x, y + delta.y);
            }

            int width, height;
            glfwGetWindowSize(m_window, &width, &height);
            ImGui::SetWindowSize(ImVec2(static_cast<float>(width), (static_cast<float>(height) - this->m_titlebarHeight)), ImGuiCond_Always);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowContentSize(ImVec2(static_cast<float>(width), static_cast<float>(height)));
            ImGui::SetWindowFontScale(1.0f);
            ImGui::SetNextWindowSizeConstraints(ImVec2(static_cast<float>(width), static_cast<float>(height)), ImVec2(static_cast<float>(width), static_cast<float>(height)));

            ImGui::ColorPicker3("##colorpicker",
                                GetConfig()->keyboard_rgb,
                                ImGuiColorEditFlags_NoAlpha);

            if (!showColorPicker) {
                glfwSetWindowShouldClose(detached_window, GLFW_TRUE);
            }

            ImGui::End();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(detached_window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(detached_window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(detached_window);

        showColorPicker = false;

        glfwMakeContextCurrent(previous_context);
        ImGui::SetCurrentContext(previous_imgui_context);
    }

    ImGui::EndGroup();
}
