#include <csignal>
#include <iostream>
#include <string>
#include <gtk/gtk.h>

#include "GUI/app.hpp"
#include "constants.hpp"
#include "utils/rgb.hpp"

void signalHandler(int signum) {
    GetKeyboard().reset();
    GetConfig().reset();
    exit(0);
}

int main(int argc, char *argv[]) {
    g_log_set_writer_func([](GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data) -> GLogWriterOutput {
        for (gsize i = 0; i < n_fields; ++i) {
            if (fields[i].key && strcmp(fields[i].key, "MESSAGE") == 0 && fields[i].value) {
                const char *msg = (const char *)fields[i].value;
                if (strstr(msg, "gtk_disable_setlocale") || strstr(msg, "gsettings-backend") ||
                    strstr(msg, "gio-vfs") || strstr(msg, "watch_") || strstr(msg, "unsetenv()")) {
                    return G_LOG_WRITER_HANDLED;
                }
            }
        }
        return g_log_writer_default(level, fields, n_fields, user_data);
    }, nullptr, nullptr);
    gtk_init(&argc, &argv);

    signal(SIGABRT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGSEGV, signalHandler);

    try {
        AjazzGUI gui("Ajazz Keyboard software");

        GetConfig() = std::make_shared<Config>();
        GetKeyboard() = std::make_shared<AK820Pro>(AJAZZ_VID, 0);

        if (GetConfig()->auto_connect && GetKeyboard()->isConnected()) {
            uint8_t r, g, b;
            RGB(GetConfig()->keyboard_rgb).toRGB(&r, &g, &b);
            GetKeyboard()->applyLightingAsync(
                GetConfig()->mode, r, g, b, GetConfig()->rainbow,
                GetConfig()->brightness, GetConfig()->speed,
                GetConfig()->direction, GetConfig()->sleep_delay);
        }

        gui.run();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}