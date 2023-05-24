#pragma once

#include "Window/Application.hpp"
#include <fmod/fmod_studio.hpp>

#include "AudioEvent.hpp"

#include <vector>

class AudioPlayer : public ImGuiApplication
{
public:
	AudioPlayer();
	~AudioPlayer();

	void RenderWindow();
	void Render();

private:
	int m_currentPage = 0;

	FMOD::Studio::System* m_System = nullptr;
	FMOD::Studio::Bank* m_masterBank = nullptr;
	FMOD::Studio::Bank* m_masterBankStrings = nullptr;

	std::string m_searchStr = {};
	std::vector<EventData*> m_eventList = {};
	std::vector<EventData*> m_eventSearched = {};

	EventDirectory* m_eventDirRoot = nullptr;
};