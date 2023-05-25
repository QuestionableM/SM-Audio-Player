#pragma once

#include <fmod/fmod_studio.hpp>

#include <unordered_map>
#include <string>
#include <vector>

struct EventParameter
{
	std::string name;
	FMOD_STUDIO_PARAMETER_ID id;

	float min_val;
	float max_val;
	float default_val;

	float value;
};

struct EventData
{
	using ParameterVector = std::vector<EventParameter>;

	std::string m_name;
	std::string m_lower_name;

	FMOD::Studio::EventDescription* m_description;
	FMOD::Studio::EventInstance* m_instance = nullptr;

	ParameterVector m_parameters;

	bool IsPlaying();
	void ApplyParameters();
	void Stop();
	void Start();

	void RenderPlaybackButtons();
	void RenderHeader();

	EventData() = default;
	~EventData();
};

struct EventDirectory
{
	using EventDirStorage = std::unordered_map<std::string, EventDirectory*>;

	std::vector<EventData*> events;
	EventDirStorage directories;
	std::string name;
	std::string path;

	void RecursiveRender();
	//Deletes all the directories owned by the current event directory
	void Clear();

	EventDirectory(const std::string& name, const std::string& path);
	~EventDirectory();
};