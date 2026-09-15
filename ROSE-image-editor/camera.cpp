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
	DestroyTexture();
}

bool Camera::Update()
{
	const auto now = std::chrono::steady_clock::now();
	const double elapsed = std::chrono::duration<double>(now - m_lastUpdate).count();
	if (elapsed < kCaptureIntervalSec)
		return false;

	glBindTexture(GL_TEXTURE_2D, m_textureID);
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
