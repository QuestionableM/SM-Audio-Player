#pragma once

#include <string>

class AudioPlayerException
{
public:
	AudioPlayerException(const std::string& title, const std::string& message)
		: m_title(title), m_message(message) {}

	AudioPlayerException(const AudioPlayerException&) = delete;
	AudioPlayerException(AudioPlayerException&&) = delete;

	~AudioPlayerException() = default;

	inline const std::string& title() const { return m_title; }
	inline const std::string& message() const { return m_message; }

private:
	std::string m_title;
	std::string m_message;
};