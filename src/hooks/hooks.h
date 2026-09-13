#pragma once
#include <EGL/egl.h>
#include <android/native_window.h>

namespace hooks {
bool init();
void shutdown();
extern bool g_running;
extern ANativeWindow* g_window;
extern int g_width, g_height;
}
