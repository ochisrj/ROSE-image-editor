#pragma once

#include <string>

class Config {
public:
    Config();
    ~Config() = default;

    const std::string& getWindowTitle() const;
    int getWindowWidth() const;
    int getWindowHeight() const;
    bool isVsyncEnabled() const;

    void setWindowTitle(const std::string& title);
    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setVsyncEnabled(bool enabled);

    void load();
    void save();

private:
    std::string m_WindowTitle;
    int m_WindowWidth;
    int m_WindowHeight;
    bool m_VsyncEnabled;
};
