#include "AudioPlayer.hpp"

#include "Popups/FmodBankLoaderPopup.hpp"
#include "Popups/AboutAppPopup.hpp"
#include "Popups/InfoPopup.hpp"

#include "AudioExceptions.hpp"
#include "Utils/String.hpp"

#include "imgui_include.hpp"

AudioPlayer::AudioPlayer()
{
	AudioPlayer::Singleton = this;

	m_eventDirRoot = new EventDirectory("ROOT", "RootPath:/");

	FMOD_RESULT fr = FMOD::Studio::System::create(&m_System);
	if (fr != FMOD_OK)
		throw AudioPlayerException("FMOD Fatal Error", "Couldn't create a FMOD Studio System");

	fr = m_System->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
	if (fr != FMOD_OK)
		throw AudioPlayerException("FMOD Fatal Error", "Couldn't initialize a FMOD Studio System");
}

AudioPlayer::~AudioPlayer()
{
	if (m_eventDirRoot)
		delete m_eventDirRoot;

	m_eventList.ClearMemory();
	m_busList.ClearMemory();
	m_vcaList.ClearMemory();

	if (m_System)
		m_System->release();
}

void AudioPlayer::LoadBankFiles()
{
	this->Clear();

	FMOD_RESULT fr = m_System->loadBankFile(m_fmodBankFile.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBank);
	if (fr != FMOD_OK)
	{
		InfoPopup::Open("FMOD Error", "Couldn't loadd a master bank file!");
		return;
	}

	fr = m_System->loadBankFile(m_fmodStringsBankFile.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBankStrings);
	if (fr != FMOD_OK)
	{
		InfoPopup::Open("FMOD Error", "Couldn't load a master bank strings file!");
		return;
	}

	int event_count;
	m_masterBank->getEventCount(&event_count);

	std::vector<FMOD::Studio::EventDescription*> v_eventDescList(static_cast<std::size_t>(event_count));
	if (m_masterBank->getEventList(v_eventDescList.data(), static_cast<int>(v_eventDescList.size()), &event_count) != FMOD_OK)
	{
		InfoPopup::Open("FMOD Error", "Couldn't get the event list");
		return;
	}

	//Load actual fmod events
	for (FMOD::Studio::EventDescription* v_cur_desc : v_eventDescList)
	{
		char v_path_buffer[512];
		int v_path_sz;
		if (v_cur_desc->getPath(v_path_buffer, sizeof(v_path_buffer), &v_path_sz) != FMOD_OK)
			continue;

		EventData* new_event = new EventData();
		new_event->m_description = v_cur_desc;

		new_event->m_name = std::string(v_path_buffer, static_cast<std::size_t>(v_path_sz));
		new_event->m_lower_name = String::ToLower(new_event->m_name);

		int event_param_count;
		if (v_cur_desc->getParameterDescriptionCount(&event_param_count) != FMOD_OK)
			continue;

		for (int b = 0; b < event_param_count; b++)
		{
			FMOD_STUDIO_PARAMETER_DESCRIPTION param_desc;
			if (v_cur_desc->getParameterDescriptionByIndex(b, &param_desc) != FMOD_OK)
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

		m_eventList.storage.push_back(new_event);
	}

	//Sort all events and create directories with events
	for (EventData* cur_event : m_eventList.storage)
	{
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

		last_directory->events.push_back(cur_event);
	}

	//Load all the busses
	int v_bus_count;
	m_masterBank->getBusCount(&v_bus_count);

	std::vector<FMOD::Studio::Bus*> v_busList(static_cast<std::size_t>(v_bus_count));
	if (m_masterBank->getBusList(v_busList.data(), static_cast<int>(v_busList.size()), &v_bus_count) != FMOD_OK)
	{
		InfoPopup::Open("FMOD Error", "Couldn't get the bus list");
		return;
	}

	for (FMOD::Studio::Bus* v_cur_bus : v_busList)
	{
		char v_path_buffer[512];
		int v_path_sz;
		if (v_cur_bus->getPath(v_path_buffer, sizeof(v_path_buffer), &v_path_sz) != FMOD_OK)
			continue;

		BusData* v_bus_data = new BusData();
		v_bus_data->name = std::string(v_path_buffer, static_cast<std::size_t>(v_path_sz));
		v_bus_data->m_lower_name = String::ToLower(v_bus_data->name);

		v_bus_data->m_bus = v_cur_bus;

		m_busList.storage.push_back(v_bus_data);
	}

	//Load all the VCA
	int v_vca_count;
	m_masterBank->getVCACount(&v_vca_count);

	std::vector<FMOD::Studio::VCA*> v_vcaList(static_cast<std::size_t>(v_vca_count));
	if (m_masterBank->getVCAList(v_vcaList.data(), static_cast<int>(v_vcaList.size()), &v_vca_count) != FMOD_OK)
	{
		InfoPopup::Open("FMOD Error", "Couldn't get the vca list");
		return;
	}

	for (FMOD::Studio::VCA* v_cur_vca : v_vcaList)
	{
		char v_path_buffer[512];
		int v_path_sz;
		if (v_cur_vca->getPath(v_path_buffer, sizeof(v_path_buffer), &v_path_sz) != FMOD_OK)
			continue;

		VcaData* v_vca_data = new VcaData();
		v_vca_data->name = std::string(v_path_buffer, static_cast<std::size_t>(v_path_sz));
		v_vca_data->m_lower_name = String::ToLower(v_vca_data->name);

		v_vca_data->m_vca = v_cur_vca;

		m_vcaList.storage.push_back(v_vca_data);
	}

	const char* v_formatted_str = String::FormatCStr("Successfully loaded:\n%llu events\n%llu BUSes\n%llu VCAs",
		m_eventList.storage.size(), m_busList.storage.size(), m_vcaList.storage.size());

	InfoPopup::Open("Success", v_formatted_str);
}

void AudioPlayer::Clear()
{
	m_eventDirRoot->Clear();

	m_eventList.ClearMemory();
	m_busList.ClearMemory();
	m_vcaList.ClearMemory();

	if (m_System->unloadAll() != FMOD_OK)
	{
		DebugErrorL("Something went wrong while trying to unload all FMOD resources");
	}
}

void RenderTextCentered(const char* text)
{
	const ImVec2 v_wnd_sz = ImGui::GetWindowSize();
	const ImVec2 v_txt_sz = ImGui::CalcTextSize(text);

	ImGui::SetCursorPos({
		(v_wnd_sz.x / 2.0f) - (v_txt_sz.x / 2.0f),
		(v_wnd_sz.y / 2.0f) - (ImGui::GetTextLineHeightWithSpacing() / 2.0f)
	});

	ImGui::Text(text);
}

void AudioPlayer::RenderEventsTab()
{
	if (m_eventList.GetVector().empty())
		RenderTextCentered(m_eventList.search_str.empty() ? "No Events" : "No Results");

	for (auto& v_cur_event : m_eventList.GetVector())
		v_cur_event->RenderHeader();
}

void AudioPlayer::RenderEventDirectoriesTab()
{
	if (m_eventDirRoot->directories.empty())
		RenderTextCentered("No Directories");

	m_eventDirRoot->RecursiveRender();
}

void AudioPlayer::RenderBUSTab()
{
	if (m_busList.GetVector().empty())
		RenderTextCentered(m_busList.search_str.empty() ? "No BUS data" : "No Results");

	for (BusData* v_cur_bus : m_busList.GetVector())
		v_cur_bus->RenderHeader();
}

void AudioPlayer::RenderVCATab()
{
	if (m_vcaList.GetVector().empty())
		RenderTextCentered(m_vcaList.search_str.empty() ? "No VCA data" : "No Results");

	for (VcaData* v_cur_bus : m_vcaList.GetVector())
		v_cur_bus->RenderHeader();
}

void AudioPlayer::RenderMenuBar()
{
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu("Program"))
	{
		if (ImGui::MenuItem("Load Bank"))
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

		ImGui::Separator();

		if (ImGui::MenuItem("About"))
			AboutPopup::Open();

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
}

using t_cur_tab_func = void (AudioPlayer::*)(void);

const static char* g_audioTabData[] = { "Events", "Sorted Events", "BUS", "VCA" };
const t_cur_tab_func g_audioFuncData[] =
{
	&AudioPlayer::RenderEventsTab,
	&AudioPlayer::RenderEventDirectoriesTab,
	&AudioPlayer::RenderBUSTab,
	&AudioPlayer::RenderVCATab
};

void AudioPlayer::RenderWindow()
{
	m_System->update();

	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::GetKeyData(ImGuiKey_F)->DownDuration == 0.0f)
		ImGui::SetKeyboardFocusHere();

	switch (m_curTabIdx)
	{
	case 0:
		m_eventList.RenderInputBox("Search Event");
		break;
	case 2:
		m_busList.RenderInputBox("Search Bus");
		break;
	case 3:
		m_vcaList.RenderInputBox("Search Vca");
		break;
	default:
		ImGui::Text("Search Not Available");
		break;
	}

	ImGui::BeginTabBar("tab_bar");
	ImGuiStyle& im_style = ImGui::GetStyle();

	ImVec4& v_tab_color_ref = im_style.Colors[ImGuiCol_Tab];
	const ImVec4 tab_normal = v_tab_color_ref;
	const ImVec4 tab_highlight = im_style.Colors[ImGuiCol_TabHovered];

	constexpr int g_audioTabDataSz = sizeof(g_audioTabData) / sizeof(char*);
	for (int a = 0; a < g_audioTabDataSz; a++)
	{
		v_tab_color_ref = (m_curTabFunc == g_audioFuncData[a]) ? tab_highlight : tab_normal;

		if (ImGui::TabItemButton(g_audioTabData[a]))
		{
			m_curTabFunc = g_audioFuncData[a];
			m_curTabIdx = a;
		}
	}

	v_tab_color_ref = tab_normal;

	ImGui::EndTabBar();

	ImVec2 size = ImGui::GetContentRegionAvail();
	size.x += ImGui::GetStyle().ItemSpacing.x;

	ImGui::BeginChild("event_list_child", size);
	(this->*m_curTabFunc)();
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

	m_lineCounter = 0;
	this->RenderMenuBar();
	this->RenderWindow();

	BankLoaderPopup::Draw();
	AboutPopup::Draw();
	InfoPopup::Draw();

	ImGui::End();
}