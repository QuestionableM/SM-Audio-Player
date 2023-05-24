#include "AudioPlayer.hpp"

#include "imgui_include.hpp"

AudioPlayer::AudioPlayer()
{
	m_eventDirRoot = new EventDirectory("ROOT", "RootPath:/");
	m_searchStr.resize(100, '\0');

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

	fr = m_System->loadBankFile("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Scrap Mechanic\\Data\\Audio\\Banks\\Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBank);
	if (fr != FMOD_OK)
	{
		DebugErrorL("Couldn't load a master bank file");
		return;
	}

	fr = m_System->loadBankFile("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Scrap Mechanic\\Data\\Audio\\Banks\\Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBankStrings);
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

		std::string event_path;
		event_path.resize(200);
		int event_path_sz;
		if (cur_desc->getPath(event_path.data(), static_cast<int>(event_path.size()), &event_path_sz) != FMOD_OK)
			continue;

		EventData* new_event = new EventData();
		new_event->m_name = event_path.substr(0, static_cast<std::size_t>(event_path_sz - 1));
		new_event->m_description = cur_desc;

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

			EventParameter new_param;
			new_param.name = std::string(param_desc.name);
			new_param.id = param_desc.id;
			new_param.default_val = param_desc.defaultvalue;
			new_param.min_val = param_desc.minimum;
			new_param.max_val = param_desc.maximum;
			new_param.value = new_param.default_val;

			new_event->m_parameters.push_back(new_param);
		}

		m_eventList.push_back(new_event);
	}


	{
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

const static char* g_audioTabData[] = { "Raw Event List", "Directories" };
void AudioPlayer::RenderWindow()
{
	m_System->update();

	if (ImGui::InputText("Search Sound", m_searchStr.data(), m_searchStr.size()))
	{
		const std::size_t str_len = strlen(m_searchStr.data());
		const std::string actual_search_str = m_searchStr.substr(0, str_len);

		m_eventSearched.clear();

		for (std::size_t a = 0; a < m_eventList.size(); a++)
		{
			EventData* cur_event = m_eventList[a];

			if (cur_event->m_name.find(actual_search_str) == std::string::npos)
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
		ImGuiWindowFlags_NoScrollWithMouse
	);

	this->RenderWindow();

	ImGui::End();
}