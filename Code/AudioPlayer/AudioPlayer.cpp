#include "AudioPlayer.hpp"

#include "Popups/FmodBankLoaderPopup.hpp"
#include "Popups/AboutAppPopup.hpp"

#include "Utils/String.hpp"

#include "imgui_include.hpp"

AudioPlayer::AudioPlayer()
{
	AudioPlayer::Singleton = this;

	m_eventDirRoot = new EventDirectory("ROOT", "RootPath:/");

	//TODO: Add proper exceptions
	FMOD_RESULT fr = FMOD::Studio::System::create(&m_System);
	if (fr != FMOD_OK)
	{
		DebugErrorL("Couldn't create a FMOD Studio System");
		return;
	}

	fr = m_System->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
	if (fr != FMOD_OK)
	{
		DebugErrorL("Couldn't initialize a FMOD Studio System");
		return;
	}
}

AudioPlayer::~AudioPlayer()
{
	if (m_eventDirRoot)
		delete m_eventDirRoot;

	for (auto& v_event : m_eventList)
		delete v_event;

	m_eventList.clear();
	m_eventSearched.clear();
	m_searchStr.clear();

	if (m_System)
		m_System->release();
}

void AudioPlayer::LoadBankFiles()
{
	this->Clear();

	FMOD_RESULT fr = m_System->loadBankFile(m_fmodBankFile.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBank);
	if (fr != FMOD_OK)
	{
		DebugErrorL("Couldn't load a master bank file");
		return;
	}

	fr = m_System->loadBankFile(m_fmodStringsBankFile.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBankStrings);
	if (fr != FMOD_OK)
	{
		DebugErrorL("Couldn't load a master bank strings file");
		return;
	}

	int event_count;
	m_masterBank->getEventCount(&event_count);

	std::vector<FMOD::Studio::EventDescription*> m_EventDescList;
	m_EventDescList.resize(static_cast<std::size_t>(event_count));

	if (m_masterBank->getEventList(m_EventDescList.data(), event_count, &event_count) != FMOD_OK)
	{
		DebugErrorL("Couldn't get the event list");
		return;
	}

	for (std::size_t a = 0; a < m_EventDescList.size(); a++)
	{
		FMOD::Studio::EventDescription* cur_desc = m_EventDescList[a];

		char v_path_buffer[512];
		int v_path_sz;
		if (cur_desc->getPath(v_path_buffer, sizeof(v_path_buffer), &v_path_sz) != FMOD_OK)
			continue;

		EventData* new_event = new EventData();
		new_event->m_description = cur_desc;

		new_event->m_name = std::string(v_path_buffer, static_cast<std::size_t>(v_path_sz));
		new_event->m_lower_name = String::ToLower(new_event->m_name);

		int event_param_count;
		if (cur_desc->getParameterDescriptionCount(&event_param_count) != FMOD_OK)
			continue;

		for (int b = 0; b < event_param_count; b++)
		{
			FMOD_STUDIO_PARAMETER_DESCRIPTION param_desc;
			if (cur_desc->getParameterDescriptionByIndex(b, &param_desc) != FMOD_OK)
				continue;

			if (param_desc.type != FMOD_STUDIO_PARAMETER_TYPE::FMOD_STUDIO_PARAMETER_GAME_CONTROLLED)
				continue;

			new_event->m_parameters.push_back(EventParameter{
				.name = param_desc.name,
				.id = param_desc.id,
				.min_val = param_desc.minimum,
				.max_val = param_desc.maximum,
				.default_val = param_desc.defaultvalue,
				.value = param_desc.defaultvalue
			});
		}

		m_eventList.push_back(new_event);
	}


	//Sort all events and create directories with events
	for (std::size_t a = 0; a < m_eventList.size(); a++)
	{
		const EventData* cur_event = m_eventList[a];

		EventDirectory* last_directory = m_eventDirRoot;

		std::size_t last_sep = 0;
		std::size_t idx = 0;
		while ((idx = cur_event->m_name.find('/', last_sep)) != std::string::npos)
		{
			const std::string category_sep = cur_event->m_name.substr(last_sep, idx - last_sep);

			EventDirectory::EventDirStorage::const_iterator iter = last_directory->directories.find(category_sep);
			if (iter == last_directory->directories.end())
			{
				EventDirectory* new_dir = new EventDirectory(category_sep, cur_event->m_name.substr(0, idx));

				last_directory->directories.insert(std::make_pair(category_sep, new_dir));
				last_directory = new_dir;
			}
			else
			{
				last_directory = iter->second;
			}

			last_sep = idx + 1;
		}

		last_directory->events.push_back(m_eventList[a]);
	}
}

void AudioPlayer::Clear()
{
	m_eventDirRoot->Clear();

	for (auto& v_event : m_eventList)
		delete v_event;

	m_eventList.clear();
	m_eventSearched.clear();
	m_searchStr.clear();

	if (m_System->unloadAll() != FMOD_OK)
	{
		DebugErrorL("Something went wrong while trying to unload all FMOD resources");
	}
}

void AudioPlayer::RenderMenuBar()
{
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Load File"))
		{
			BankLoaderPopup::Open(
				[this, &r_bank_path = m_fmodBankFile, &r_bank_strings_path = m_fmodStringsBankFile]
				(const std::string& bank_path, const std::string& bank_strings_path) -> void {
					DebugOutL("Bank: ", bank_path, "\nBank Strings: ", bank_strings_path);
				
					r_bank_path = bank_path;
					r_bank_strings_path = bank_strings_path;

					this->LoadBankFiles();
				}
			);
		}

		if (ImGui::MenuItem("Clear Window"))
			this->Clear();

		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("About"))
		AboutPopup::Open();

	ImGui::EndMenuBar();
}

const static char* g_audioTabData[] = { "Raw Event List", "Directories" };
void AudioPlayer::RenderWindow()
{
	m_System->update();

	if (ImGui::InputText("Search Sound", &m_searchStr))
	{
		const std::string v_lower_search = String::ToLower(m_searchStr);
		m_eventSearched.clear();

		for (std::size_t a = 0; a < m_eventList.size(); a++)
		{
			EventData* cur_event = m_eventList[a];

			if (cur_event->m_name.find(v_lower_search) == std::string::npos)
				continue;

			m_eventSearched.push_back(cur_event);
		}
	}

	ImGui::BeginTabBar("tab_bar");
	ImGuiStyle& im_style = ImGui::GetStyle();

	const ImVec4 tab_normal = im_style.Colors[ImGuiCol_Tab];
	const ImVec4 tab_highlight = im_style.Colors[ImGuiCol_TabHovered];

	constexpr int g_audioTabDataSz = sizeof(g_audioTabData) / sizeof(char*);
	for (int a = 0; a < g_audioTabDataSz; a++)
	{
		im_style.Colors[ImGuiCol_Tab] = (a == m_currentPage) ? tab_highlight : tab_normal;

		if (ImGui::TabItemButton(g_audioTabData[a]))
			m_currentPage = a;
	}

	im_style.Colors[ImGuiCol_Tab] = tab_normal;

	ImGui::EndTabBar();

	ImGui::BeginChild("event_list_child", { 0.0f, 0.0f }, true);

	switch (m_currentPage)
	{
	case 0:
		{
			std::vector<EventData*>& cur_event_vec = (m_eventSearched.empty() ? m_eventList : m_eventSearched);
			for (std::size_t a = 0; a < cur_event_vec.size(); a++)
				cur_event_vec[a]->RenderHeader(a);

			break;
		}
	case 1:
		m_eventDirRoot->RecursiveRender(0);
		break;
	}

	ImGui::EndChild();
}

void AudioPlayer::Render()
{
	ImGui::SetNextWindowPos({ 0.0f, 0.0f });	
	ImGui::SetNextWindowSize({ (float)m_window->GetWidth(), (float)m_window->GetHeight() });

	ImGui::Begin(
		"##sm_audio_player_window",
		nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_MenuBar
	);

	this->RenderMenuBar();
	this->RenderWindow();

	BankLoaderPopup::Draw();
	AboutPopup::Draw();

	ImGui::End();
}