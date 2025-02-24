//
// Created by taxmachine on 22/02/25.
//

#ifndef APP_HPP
#define APP_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <string>

#include "../keyboards/ak820pro.hpp"

class AjazzGUI {
public:
    explicit AjazzGUI(std::string  applicationName, const AK820Pro& keyboard);
    ~AjazzGUI();
    void run();

private:
    void initWindow();
    void initIMGUI() const;
    void mainLoop();
    void render();

    void ModeWindow();

    void changeMode();

    std::string m_applicationName;
    GLFWwindow* m_window{};
    float m_titlebarHeight{};
    AK820Pro m_keyboard;
    AK820Pro::LightingMode m_mode;
    float m_keyboardRGBColor[3] = {1.0f, 0.0f, 0.0f};
    bool m_rainbow = true;
    int m_brightness = 1;
    int m_speed = 1;
    AK820Pro::DIRECTION m_direction;
};

#endif //APP_HPP
