#include "camera.h"

#include <chrono>

#include <glad/glad.h>

namespace
{
	const double kCaptureIntervalSec = 1.0 / 30.0;
}

Camera::Camera()
	: m_lastUpdate(std::chrono::steady_clock::now())
{
}

Camera::~Camera()
{
	Close();
}

std::vector<std::string> Camera::DetectDevices()
{
	std::vector<std::string> devices;
	for (int i = 0; i < 10; ++i)
	{
		cv::VideoCapture probe(i, cv::CAP_MSMF);
		if (probe.isOpened())
		{
			const int w = (int)probe.get(cv::CAP_PROP_FRAME_WIDTH);
			const int h = (int)probe.get(cv::CAP_PROP_FRAME_HEIGHT);
			devices.push_back("Camera " + std::to_string(i) + " (" + std::to_string(w) + "x" + std::to_string(h) + ")");
		}
		probe.release();
	}
	return devices;
}

bool Camera::Open(int deviceIndex)
{
	Close();

	if (deviceIndex < 0)
		return false;

	m_capture.open(deviceIndex, cv::CAP_MSMF);
	if (!m_capture.isOpened())
		return false;

	m_capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
	m_capture.set(cv::CAP_PROP_FPS, 30);

	cv::Mat frame;
	if (!m_capture.read(frame) || frame.empty())
	{
		m_capture.release();
		return false;
	}

	cv::cvtColor(frame, m_frameRGB, cv::COLOR_BGR2RGB);
	CreateTexture(m_frameRGB.cols, m_frameRGB.rows);
	m_opened = true;
	m_lastUpdate = std::chrono::steady_clock::now();
	return true;
}

void Camera::Close()
{
	if (m_capture.isOpened())
		m_capture.release();
	DestroyTexture();
	m_frameRGB.release();
	m_opened = false;
}

bool Camera::Update()
{
	if (!m_opened || !m_capture.isOpened())
		return false;

	const auto now = std::chrono::steady_clock::now();
	const double elapsed = std::chrono::duration<double>(now - m_lastUpdate).count();
	if (elapsed < kCaptureIntervalSec)
		return false;

	cv::Mat frame;
	if (!m_capture.read(frame) || frame.empty())
		return false;

	cv::cvtColor(frame, m_frameRGB, cv::COLOR_BGR2RGB);

	if (m_frameRGB.cols != m_width || m_frameRGB.rows != m_height)
		CreateTexture(m_frameRGB.cols, m_frameRGB.rows);

	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_frameRGB.cols, m_frameRGB.rows, GL_RGB, GL_UNSIGNED_BYTE, m_frameRGB.data);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_lastUpdate = now;
	return true;
}

void Camera::CreateTexture(int width, int height)
{
	if (m_textureID != 0)
		DestroyTexture();

	m_width = width;
	m_height = height;

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Camera::DestroyTexture()
{
	if (m_textureID != 0)
	{
		glDeleteTextures(1, &m_textureID);
		m_textureID = 0;
	}
	m_width = 0;
	m_height = 0;
}