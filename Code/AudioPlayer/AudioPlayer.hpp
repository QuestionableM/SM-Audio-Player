#pragma once

#include "Window/Application.hpp"
#include <fmod/fmod_studio.hpp>

#include "SearchVector.hpp"
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

	//Tab rendering functions
	void RenderEventsTab();
	void RenderEventDirectoriesTab();
	void RenderBUSTab();
	void RenderVCATab();

	void RenderMenuBar();
	void RenderWindow();
	void Render();

private:
	using t_current_tab_func_ptr = void (AudioPlayer::*)(void);
	t_current_tab_func_ptr m_curTabFunc = &AudioPlayer::RenderEventsTab;
	int m_curTabIdx = 0;

	FMOD::Studio::System* m_System = nullptr;
	FMOD::Studio::Bank* m_masterBank = nullptr;
	FMOD::Studio::Bank* m_masterBankStrings = nullptr;

	std::string m_fmodBankFile;
	std::string m_fmodStringsBankFile;

	SearchVector<EventData> m_eventList;
	SearchVector<BusData> m_busList;
	SearchVector<VcaData> m_vcaList;

	EventDirectory* m_eventDirRoot = nullptr;
};