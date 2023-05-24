#include "AudioEvent.hpp"

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

	for (ParameterVector::const_iterator iter = m_parameters.begin(); iter != m_parameters.end(); iter++)
	{
		const EventParameter& v_curParam = (*iter);
		if (m_instance->setParameterByID(v_curParam.id, v_curParam.value) != FMOD_OK)
		{
			DebugErrorL("Couldn't set the value for: ", v_curParam.name);
		}
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

	if (m_description->createInstance(&m_instance) == FMOD_OK)
	{
		DebugOutL("Event created: ", m_name);

		this->ApplyParameters();
		m_instance->start();
	}
	else
	{
		DebugErrorL("Couldn't create event instance: ", m_name);
	}
}

void EventData::RenderHeader(const std::size_t& id)
{
	if (!ImGui::CollapsingHeader(m_name.c_str()))
		return;

	ImGui::Indent(15.0f);
	ImGui::PushID(static_cast<int>(id));
	if (ImGui::Button("Play Sound"))
		this->Start();

	if (this->IsPlaying())
	{
		ImGui::SameLine();
		if (ImGui::Button("Stop Sound"))
			this->Stop();

		int event_length;
		if (m_description->getLength(&event_length) == FMOD_OK)
		{
			int event_playback_pos;
			if (m_instance->getTimelinePosition(&event_playback_pos) == FMOD_OK)
			{
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
		}
	}

	for (ParameterVector::iterator iter = m_parameters.begin(); iter != m_parameters.end(); iter++)
	{
		EventParameter& cur_param = (*iter);

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



void EventDirectory::RecursiveRender(const std::size_t& recursion_depth)
{
	std::size_t dir_counter = 0;

	for (EventDirStorage::iterator iter = this->directories.begin(); iter != this->directories.end(); iter++)
	{
		ImGui::PushID(static_cast<int>(dir_counter + recursion_depth));

		if (ImGui::CollapsingHeader(iter->second->name.c_str()))
		{
			ImGui::Indent(20.0f);

			iter->second->RecursiveRender(recursion_depth + 1);

			for (std::size_t a = 0; a < iter->second->events.size(); a++)
				iter->second->events[a]->RenderHeader(a);

			ImGui::Unindent(20.0f);
		}

		ImGui::PopID();

		dir_counter++;
	}
}

EventDirectory::EventDirectory(const std::string& name, const std::string& path)
{
	this->name = name;
	this->path = path;
}

EventDirectory::~EventDirectory()
{
	for (EventDirStorage::iterator i = this->directories.begin(); i != this->directories.end(); i++)
		delete i->second;
}