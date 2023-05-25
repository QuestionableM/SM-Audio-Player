#include "InfoPopup.hpp"

#include "AudioPlayer/AudioPlayer.hpp"

#include "Utils/String.hpp"
#include "imgui_include.hpp"

void InfoPopup::Open(const std::string& title, const std::string& message)
{
	const char* v_popup_title = String::FormatCStr("%s###info_popup", title.c_str());
	if (ImGui::IsPopupOpen(v_popup_title))
		return;

	InfoPopup::ShouldOpen = true;

	InfoPopup::PopupTitle = title;
	InfoPopup::PopupMessage = message;
}

void InfoPopup::Draw()
{
	const char* v_popup_title = String::FormatCStr("%s###info_popup", InfoPopup::PopupTitle.c_str());

	if (InfoPopup::ShouldOpen)
	{
		InfoPopup::ShouldOpen = false;
		ImGui::OpenPopup(v_popup_title, ImGuiPopupFlags_NoOpenOverExistingPopup);
	}

	ImGui::SetNextWindowSize({ 300.0f, -FLT_MIN });

	if (!ImGui::BeginPopupModal(v_popup_title, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		return;

	const ImVec2 v_popup_sz = ImGui::GetWindowSize();
	ImGui::SetWindowPos({
		(static_cast<float>(APP_PTR()->m_window->GetWidth()) / 2.0f) - (v_popup_sz.x / 2.0f),
		(static_cast<float>(APP_PTR()->m_window->GetHeight()) / 2.0f) - (v_popup_sz.y / 2.0f)
	});

	ImGui::TextWrapped(InfoPopup::PopupMessage.c_str());

	if (ImGui::Button("Ok"))
		ImGui::CloseCurrentPopup();

	ImGui::EndPopup();
}