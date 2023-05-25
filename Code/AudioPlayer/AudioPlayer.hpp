#pragma once

#include "Window/Application.hpp"
#include <fmod/fmod_studio.hpp>

#include "AudioEvent.hpp"

#include <vector>

#define APP_PTR() AudioPlayer::Singleton

class AudioPlayer : public ImGuiApplication
{
	friend class BankLoaderPopup;
	friend class AboutPopup;
	friend class InfoPopup;

public:
	inline static AudioPlayer* Singleton = nullptr;
	int m_lineCounter = 0;

	AudioPlayer();
	~AudioPlayer();

	void LoadBankFiles();
	void Clear();

	void RenderMenuBar();
	void RenderWindow();
	void Render();

private:
	int m_currentPage = 0;

	FMOD::Studio::System* m_System = nullptr;
	FMOD::Studio::Bank* m_masterBank = nullptr;
	FMOD::Studio::Bank* m_masterBankStrings = nullptr;

	std::string m_fmodBankFile;
	std::string m_fmodStringsBankFile;

	std::string m_searchStr = {};
	std::vector<EventData*> m_eventList = {};
	std::vector<EventData*> m_eventSearched = {};

	EventDirectory* m_eventDirRoot = nullptr;
};