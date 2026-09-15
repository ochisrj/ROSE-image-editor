#pragma once
#include <chrono>
#include <string>
#include <vector>

struct GLFWwindow;

class Camera
{
public:
	Camera();
	~Camera();

	bool Update();
	unsigned int GetTextureID() const { return m_textureID; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	void CreateTexture(int width, int height);
	void DestroyTexture();

	unsigned int     m_textureID = 0;
	int              m_width = 0;
	int              m_height = 0;
	bool             m_opened = false;
	std::chrono::steady_clock::time_point m_lastUpdate;
};
