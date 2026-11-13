//
// Created by taxmachine on 22/02/25.
// Modified by Vixcho on 13/10/26.
//

#include "app.hpp"

#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include "../constants.hpp"
#include "../utils/logger.hpp"
#include "../utils/i18n.hpp"
#include <cstdlib>
#include <imgui.h>
#include <iostream>
#include <utility>

static void glfw_error_callback(const int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
  Logger::getInstance().error("GLFW Error " + std::to_string(error) + ": " +
                              std::string(description));
}

AjazzGUI::AjazzGUI(std::string applicationName)
    : m_applicationName(std::move(applicationName)) {
  Logger::getInstance().info("Iniciando Ajazz Keyboard Software Linux...");
  initWindow();
  initIMGUI();
}

AjazzGUI::~AjazzGUI() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

void AjazzGUI::run() { mainLoop(); }

void AjazzGUI::initWindow() {
  glfwSetErrorCallback(glfw_error_callback);
  glfwInit();
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
  this->m_window = glfwCreateWindow(960, 760, this->m_applicationName.c_str(),
                                    nullptr, nullptr);
  if (!this->m_window) {
    Logger::getInstance().error("Error creando ventana GLFW");
    return;
  }

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1);
  Logger::getInstance().success("Ventana gráfica inicializada correctamente.");
}

void AjazzGUI::initIMGUI() const {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigWindowsResizeFromEdges = false;

  ImGuiStyle &style = ImGui::GetStyle();
  ImGui::StyleColorsDark();

  style.WindowRounding = 6.0f;
  style.ChildRounding = 6.0f;
  style.FrameRounding = 4.0f;
  style.PopupRounding = 4.0f;
  style.ScrollbarRounding = 6.0f;
  style.GrabRounding = 4.0f;
  style.TabRounding = 4.0f;
  style.WindowPadding = ImVec2(16, 16);
  style.FramePadding = ImVec2(10, 6);
  style.ItemSpacing = ImVec2(10, 8);

  ImVec4 *colors = style.Colors;
  colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.12f, 0.14f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.15f, 0.18f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4(0.22f, 0.24f, 0.28f, 0.50f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.27f, 0.32f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.32f, 0.38f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.20f, 0.23f, 0.28f, 1.00f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.34f, 0.42f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.43f, 0.54f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.22f, 0.26f, 0.32f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.36f, 0.45f, 1.00f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.46f, 0.58f, 1.00f);

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
    glClearColor(0.09f, 0.10f, 0.12f, 1.00f);
  }
}

void AjazzGUI::renderTopToolbar() {
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.11f, 1.00f));
  ImGui::BeginChild("##TopToolbar", ImVec2(0, 52), false,
                    ImGuiWindowFlags_NoScrollbar |
                        ImGuiWindowFlags_NoScrollWithMouse);

  auto renderTabButton = [this](const char *label, Tab tab) {
    bool isActive = (m_activeTab == tab);
    if (isActive) {
      ImGui::PushStyleColor(ImGuiCol_Button,
                            ImVec4(0.22f, 0.40f, 0.65f, 1.00f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    } else {
      ImGui::PushStyleColor(ImGuiCol_Button,
                            ImVec4(0.14f, 0.16f, 0.19f, 0.80f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.82f, 0.86f, 1.00f));
    }

    if (ImGui::Button(label, ImVec2(0, 36))) {
      m_activeTab = tab;
    }

    ImGui::PopStyleColor(2);
    ImGui::SameLine(0, 8);
  };

  ImGui::SetCursorPosY(8);
  ImGui::SetCursorPosX(12);
  renderTabButton(tr("tab_lighting"), Tab::LIGHTING);
  renderTabButton(tr("tab_individual_leds"), Tab::INDIVIDUAL_LEDS);
  renderTabButton(tr("tab_macros"), Tab::MACROS);
  renderTabButton(tr("tab_tft_screen"), Tab::TFT_SCREEN);
  renderTabButton(tr("tab_theme"), Tab::THEME);
  renderTabButton(tr("tab_settings"), Tab::CONFIG);
  renderTabButton(tr("tab_device_status"), Tab::DEVICE);
  renderTabButton(tr("tab_logs"), Tab::LOGS);



  ImGui::EndChild();
  ImGui::PopStyleColor();
}

void AjazzGUI::render() {
  int width, height;
  glfwGetWindowSize(m_window, &width, &height);

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(
      ImVec2(static_cast<float>(width), static_cast<float>(height)),
      ImGuiCond_Always);

  ImGui::Begin("##MainWindow", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoNavFocus);

  renderTopToolbar();

  ImGui::SetCursorPosY(60);
  ImGui::BeginChild("##ContentArea", ImVec2(0, 0), true);

  switch (m_activeTab) {
  case Tab::LIGHTING:
    this->ModeWindow();
    break;
  case Tab::INDIVIDUAL_LEDS:
    this->IndividualLEDsWindow();
    break;
  case Tab::MACROS:
    this->MacrosWindow();
    break;
  case Tab::TFT_SCREEN:
    this->TFTScreenWindow();
    break;
  case Tab::THEME:
    this->ThemeEditorWindow();
    break;
  case Tab::CONFIG:
    this->ConfigWindow();
    break;
  case Tab::DEVICE:
    this->DeviceStatusWindow();
    break;
  case Tab::LOGS:
    this->LogsWindow();
    break;
  }

  ImGui::EndChild();
  ImGui::End();
}
