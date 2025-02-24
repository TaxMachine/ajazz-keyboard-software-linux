//
// Created by taxmachine on 23/02/25.
//

#include <future>

#include "../app.hpp"

#include <imgui.h>
#include <iostream>
#include <thread>

void AjazzGUI::changeMode() {
    try {
        this->m_keyboard.setMode(
                this->m_mode,
                static_cast<uint8_t>(this->m_keyboardRGBColor[0] * 255.0f),
                static_cast<uint8_t>(this->m_keyboardRGBColor[1] * 255.0f),
                static_cast<uint8_t>(this->m_keyboardRGBColor[2] * 255.0f),
                this->m_rainbow,
                this->m_brightness,
                this->m_speed,
                this->m_direction);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void AjazzGUI::ModeWindow() {
    ImGui::BeginGroup();

    static int selectedMode = 1;
    ImGui::BeginGroup();
        if (ImGui::RadioButton("LED off", &selectedMode, 0)) {
            this->m_mode = AK820Pro::LED_OFF;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }

        if (ImGui::RadioButton("Static", &selectedMode, 1)) {
            this->m_mode = AK820Pro::STATIC;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Single on", &selectedMode, 2)) {
            this->m_mode = AK820Pro::SINGLE_ON;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Single off", &selectedMode, 3)) {
            this->m_mode = AK820Pro::SINGLE_OFF;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Glittering", &selectedMode, 4)) {
            this->m_mode = AK820Pro::GLITTERING;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }



        if (ImGui::RadioButton("Falling", &selectedMode, 5)) {
            this->m_mode = AK820Pro::FALLING;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Colourful", &selectedMode, 6)) {
            this->m_mode = AK820Pro::COLOURFUL;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Breath", &selectedMode, 7)) {
            this->m_mode = AK820Pro::BREATH;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Spectrum", &selectedMode, 8)) {
            this->m_mode = AK820Pro::SPECTRUM;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }


        if (ImGui::RadioButton("Outward", &selectedMode, 9)) {
            this->m_mode = AK820Pro::OUTWARD;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Scrolling", &selectedMode, 10)) {
            this->m_mode = AK820Pro::SCROLLING;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Rolling", &selectedMode, 11)) {
            this->m_mode = AK820Pro::ROLLING;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Rotating", &selectedMode, 12)) {
            this->m_mode = AK820Pro::ROTATING;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }


        if (ImGui::RadioButton("Explode", &selectedMode, 13)) {
            this->m_mode = AK820Pro::EXPLODE;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Launch", &selectedMode, 14)) {
            this->m_mode = AK820Pro::LAUNCH;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Ripples", &selectedMode, 15)) {
            this->m_mode = AK820Pro::RIPPLES;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Flowing", &selectedMode, 16)) {
            this->m_mode = AK820Pro::FLOWING;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }

        if (ImGui::RadioButton("Pulsating", &selectedMode, 17)) {
            this->m_mode = AK820Pro::PULSATING;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Tilt", &selectedMode, 18)) {
            this->m_mode = AK820Pro::TILT;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
        ImGui::SameLine(0, 10);
        if (ImGui::RadioButton("Shuttle", &selectedMode, 19)) {
            this->m_mode = AK820Pro::SHUTTLE;
            std::future<void> future = std::async([this] {
                this->changeMode();
            });
        }
    ImGui::EndGroup();

    ImGui::SameLine(0, 15);

    if (ImGui::ColorPicker3("", this->m_keyboardRGBColor, ImGuiColorEditFlags_NoSidePreview |
                                                                                ImGuiColorEditFlags_NoSmallPreview |
                                                                                ImGuiColorEditFlags_NoOptions |
                                                                                ImGuiColorEditFlags_PickerHueWheel)) {
        std::future<void> future = std::async([this] {
            this->changeMode();
        });
    }

    ImGui::BeginGroup();
    if (ImGui::Checkbox("Rainbow", &this->m_rainbow)) {
        std::future<void> future = std::async([this] {
            this->changeMode();
        });
    }

    if (ImGui::SliderInt("Brightness", &this->m_brightness, 0, 5)) {
        std::future<void> future = std::async([this] {
            this->changeMode();
        });
    }

    if (ImGui::SliderInt("Speed", &this->m_speed, 0, 5)) {
        std::future<void> future = std::async([this] {
            this->changeMode();
        });
    }

    ImGui::BeginGroup();
    static int selectedTime = 0;

    if (ImGui::RadioButton("No Sleep", &selectedTime, 0)) {
        std::future<void> future = std::async([this] {
            this->m_keyboard.setSleepTime(AK820Pro::NONE);
        });
    }

    ImGui::SameLine(0, 10);
    if (ImGui::RadioButton("1 Minute", &selectedTime, 1)) {
        std::future<void> future = std::async([this] {
            this->m_keyboard.setSleepTime(AK820Pro::ONE_MIN);
        });
    }

    ImGui::SameLine(0, 10);
    if (ImGui::RadioButton("5 Minute", &selectedTime, 2)) {
        std::future<void> future = std::async([this] {
            this->m_keyboard.setSleepTime(AK820Pro::FIVE_MIN);
        });
    }

    ImGui::SameLine(0, 10);
    if (ImGui::RadioButton("30 Minutes", &selectedTime, 3)) {
        std::future<void> future = std::async([this] {
            this->m_keyboard.setSleepTime(AK820Pro::THIRTY_MIN);
        });
    }

    ImGui::EndGroup();

    ImGui::EndGroup();

    ImGui::EndGroup();
}
