//
// Created by taxmachine on 22/02/25.
//

#ifndef APP_HPP
#define APP_HPP

#include <GLFW/glfw3.h>
#include <string>
#include <map>
#include <vector>

#include "../keyboards/ak820pro.hpp"

class AjazzGUI {
public:
    explicit AjazzGUI(std::string  applicationName);
    ~AjazzGUI();
    void run();

private:
    void initWindow();
    void initIMGUI() const;
    void mainLoop();
    void render();

    void ModeWindow();
    void TFTScreenWindow();
    void MacrosWindow();
    void ThemeEditorWindow();

    void changeMode();

    std::string m_applicationName;
    GLFWwindow* m_window{};
    float m_titlebarHeight{};

    std::map<AK820Pro::LightingMode, const char*> LIGHT_MODES = {
        {AK820Pro::LED_OFF, "LED off"},
        {AK820Pro::STATIC, "Static"},
        {AK820Pro::SINGLE_ON, "Single on"},
        {AK820Pro::SINGLE_OFF, "Single off"},
        {AK820Pro::GLITTERING, "Glittering"},
        {AK820Pro::FALLING, "Falling"},
        {AK820Pro::COLOURFUL, "Colourful"},
        {AK820Pro::BREATH, "Breath"},
        {AK820Pro::SPECTRUM, "Spectrum"},
        {AK820Pro::OUTWARD, "Outward"},
        {AK820Pro::SCROLLING, "Scrolling"},
        {AK820Pro::ROLLING, "Rolling"},
        {AK820Pro::ROTATING, "Rotating"},
        {AK820Pro::EXPLODE, "Explode"},
        {AK820Pro::LAUNCH, "Launch"},
        {AK820Pro::RIPPLES, "Ripples"},
        {AK820Pro::FLOWING, "Flowing"},
        {AK820Pro::PULSATING, "Pulsating"},
        {AK820Pro::TILT, "Tilt"},
        {AK820Pro::SHUTTLE, "Shuttle"}
    };

    std::map<AK820Pro::Direction, const char*> DIRECTIONS = {
        {AK820Pro::LEFT, "Left"},
        {AK820Pro::UP, "Up"},
        {AK820Pro::DOWN, "Down"},
        {AK820Pro::RIGHT, "Right"}
    };

    std::map<AK820Pro::LightSleepTime, const char*> SLEEP_DELAY = {
        {AK820Pro::NONE, "No Sleep"},
        {AK820Pro::ONE_MIN, "1 minute"},
        {AK820Pro::FIVE_MIN, "5 minutes"},
        {AK820Pro::THIRTY_MIN, "30 minutes"}
    };

    std::map<AK820Pro::LightingMode, std::vector<AK820Pro::Direction>> DIRECTIONS_PER_MODE = {
        {AK820Pro::SCROLLING, {AK820Pro::UP, AK820Pro::DOWN}},
        {AK820Pro::ROLLING, {AK820Pro::LEFT, AK820Pro::RIGHT}},
        {AK820Pro::FLOWING, {AK820Pro::LEFT, AK820Pro::RIGHT}},
        {AK820Pro::TILT, {AK820Pro::LEFT, AK820Pro::RIGHT}}
    };
};

#endif //APP_HPP
