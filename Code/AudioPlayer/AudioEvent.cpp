#include "AudioEvent.hpp"
#include "AudioPlayer/AudioPlayer.hpp"

#include "Utils/Console.hpp"

#include "imgui_include.hpp"

bool EventData::IsPlaying()
{
	if (!m_instance) return false;

	FMOD_STUDIO_PLAYBACK_STATE state;
	if (m_instance->getPlaybackState(&state) != FMOD_OK)
		return false;

	return (state != FMOD_STUDIO_PLAYBACK_STOPPED);
}

void EventData::ApplyParameters()
{
	if (!m_instance) return;

	for (const auto& v_curParam : m_parameters)
	{
		if (m_instance->setParameterByID(v_curParam.id, v_curParam.value) == FMOD_OK)
			continue;

		DebugErrorL("Couldn't set the value for: ", v_curParam.name);
	}
}

void EventData::Stop()
{
	if (this->IsPlaying())
	{
		m_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
}

void EventData::Start()
{
	if (m_instance)
	{
		DebugOutL("Event Started: ", m_name);
		m_instance->start();
		return;
	}

	if (m_description->createInstance(&m_instance) != FMOD_OK)
	{
		DebugErrorL("Couldn't create event instance: ", m_name);
		return;
	}

	DebugOutL("Event created: ", m_name);

	this->ApplyParameters();
	m_instance->start();
}

void EventData::RenderPlaybackButtons()
{
	if (!this->IsPlaying())
		return;

	ImGui::SameLine();
	if (ImGui::Button("Stop Sound"))
		this->Stop();

	int event_length;
	if (m_description->getLength(&event_length) != FMOD_OK)
		return;

	int event_playback_pos;
	if (m_instance->getTimelinePosition(&event_playback_pos) != FMOD_OK)
		return;

	int event_changer_test = event_playback_pos;
	ImGui::SameLine();
	if (ImGui::SliderInt("Playback", &event_changer_test, 0, event_length, "%d", ImGuiSliderFlags_None))
	{
		if (m_instance->setTimelinePosition(event_changer_test) == FMOD_OK)
		{
			DebugOutL("Playback set to ", event_changer_test, " / ", event_length);
		}
	}
}

void EventData::RenderHeader()
{
	if (!ImGui::CollapsingHeader(m_name.c_str()))
		return;

	ImGui::Indent(15.0f);
	ImGui::PushID(APP_PTR()->m_lineCounter++);
	if (ImGui::Button("Play Sound"))
		this->Start();

	ImGui::SameLine();

	if (ImGui::Button("Copy Path"))
		ImGui::SetClipboardText(m_name.c_str());

	this->RenderPlaybackButtons();

	for (auto& cur_param : m_parameters)
	{
		if (!ImGui::SliderFloat(cur_param.name.c_str(), &cur_param.value, cur_param.min_val, cur_param.max_val, "%.5f", ImGuiSliderFlags_None))
			continue;

		if (!m_instance)
			continue;

		if (m_instance->setParameterByID(cur_param.id, cur_param.value) != FMOD_OK)
		{
			DebugErrorL("Couldn't set the parameter by id!");
			continue;
		}

		DebugOutL("Parameter ", cur_param.name, " set to ", cur_param.value);
	}

	ImGui::PopID();
	ImGui::Unindent(15.0f);
}

EventData::~EventData()
{
	if (m_instance)
	{
		this->Stop();
		m_instance->release();
	}
}



void EventDirectory::RecursiveRender()
{
	for (auto& [dir_name, dir_ptr] : this->directories)
	{
		ImGui::PushID(APP_PTR()->m_lineCounter++);

		if (ImGui::CollapsingHeader(dir_ptr->name.c_str()))
		{
			ImGui::Indent(20.0f);

			dir_ptr->RecursiveRender();

			for (auto& v_cur_event : dir_ptr->events)
				v_cur_event->RenderHeader();

			ImGui::Unindent(20.0f);
		}

		ImGui::PopID();
	}
}

void EventDirectory::Clear()
{
	for (auto& [dir_name, dir_ptr] : this->directories)
		delete dir_ptr;

	this->directories.clear();
}

EventDirectory::EventDirectory(const std::string& name, const std::string& path)
{
	this->name = name;
	this->path = path;
}

EventDirectory::~EventDirectory()
{
	this->Clear();
}