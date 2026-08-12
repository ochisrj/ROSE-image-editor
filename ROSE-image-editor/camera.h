#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>
#include <string>
#include <vector>

struct GLFWwindow;

class Camera
{
public:
	Camera();
	~Camera();

	static std::vector<std::string> DetectDevices();

	bool Open(int deviceIndex);
	void Close();
	bool IsOpen() const { return m_opened; }

	bool Update();
	unsigned int GetTextureID() const { return m_textureID; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	void CreateTexture(int width, int height);
	void DestroyTexture();

	cv::VideoCapture m_capture;
	cv::Mat          m_frameRGB;
	unsigned int     m_textureID = 0;
	int              m_width = 0;
	int              m_height = 0;
	bool             m_opened = false;
	std::chrono::steady_clock::time_point m_lastUpdate;
};