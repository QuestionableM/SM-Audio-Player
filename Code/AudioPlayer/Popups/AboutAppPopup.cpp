#include "AboutAppPopup.hpp"

#include "AudioPlayer/AudioPlayer.hpp"
#include "imgui_include.hpp"

#include <shellapi.h>

#define AboutPopup_Name "About###about_app_popup"

void AboutPopup::Open()
{
	if (ImGui::IsPopupOpen(AboutPopup_Name))
		return;

	AboutPopup::ShouldOpen = true;
}

void TextLink(const char* label, const char* link_str)
{
	ImColor v_col = IM_COL32(127, 127, 255, 255);

	ImGui::TextColored(v_col, label);

	if (ImGui::IsItemClicked())
		ShellExecuteA(0, 0, link_str, 0, 0, SW_SHOW);

	const ImVec2 line_end = ImGui::GetItemRectMax();
	const ImVec2 line_start =
	{
		ImGui::GetItemRectMin().x,
		line_end.y
	};

	ImGui::GetWindowDrawList()->AddLine(line_start, line_end, v_col, 1.0f);
}

void AboutPopup::Draw()
{
	if (AboutPopup::ShouldOpen)
	{
		AboutPopup::ShouldOpen = false;
		ImGui::OpenPopup(AboutPopup_Name, ImGuiPopupFlags_NoOpenOverExistingPopup);
	}

	ImGui::SetNextWindowSize({ 350.0f, -FLT_MIN });

	if (!ImGui::BeginPopupModal(AboutPopup_Name, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		return;

	const ImVec2 v_popup_sz = ImGui::GetWindowSize();
	ImGui::SetWindowPos({
		(static_cast<float>(APP_PTR()->m_window->GetWidth()) / 2.0f) - (v_popup_sz.x / 2.0f),
		(static_cast<float>(APP_PTR()->m_window->GetHeight() / 2.0f) - (v_popup_sz.y / 2.0f))
	});

	ImGui::TextWrapped("SM Audio Player allows you to test the audio parameters in real time!");

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Created by:");
	ImGui::SameLine();
	TextLink("Questionable Mark", "https://github.com/QuestionableM");

	ImGui::Text("Powered by:");
	ImGui::SameLine();
	TextLink("FMOD", "https://www.fmod.com/");
	ImGui::SameLine();
	TextLink("ImGui", "https://github.com/ocornut/imgui");

	ImGui::Text("Build Timestamp: " __TIMESTAMP__);

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Escape))
		ImGui::CloseCurrentPopup();

	ImGui::EndPopup();
}