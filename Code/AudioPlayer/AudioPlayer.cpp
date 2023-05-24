#include "AudioPlayer.hpp"

#include "imgui_include.hpp"

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



	ImGui::End();
}