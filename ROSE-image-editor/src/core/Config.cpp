#include "core/Config.h"

Config::Config()
    : m_WindowTitle("ROSE Image Editor")
    , m_WindowWidth(1280)
    , m_WindowHeight(720)
    , m_VsyncEnabled(true)
{
}

const std::string& Config::getWindowTitle() const {
    return m_WindowTitle;
}

int Config::getWindowWidth() const {
    return m_WindowWidth;
}

int Config::getWindowHeight() const {
    return m_WindowHeight;
}

bool Config::isVsyncEnabled() const {
    return m_VsyncEnabled;
}

void Config::setWindowTitle(const std::string& title) {
    m_WindowTitle = title;
}

void Config::setWindowWidth(int width) {
    m_WindowWidth = width;
}

void Config::setWindowHeight(int height) {
    m_WindowHeight = height;
}

void Config::setVsyncEnabled(bool enabled) {
    m_VsyncEnabled = enabled;
}

void Config::load() {
}

void Config::save() {
}
